#include "stdafx.hpp"

int main( int argc, char** argv )
{
	std::atexit( []()
	{
		std::cin.get();
	} );

	HKEY h_key = nullptr;
	if ( RegOpenKeyExA( HKEY_CURRENT_USER, "Software\\Valve\\Steam", 0, KEY_QUERY_VALUE, &h_key ) != ERROR_SUCCESS )
	{
		RegCloseKey( h_key );

		std::cout << "failed to find steam registry" << std::endl;
		return EXIT_FAILURE;
	}

	char steam_path_reg[MAX_PATH]; steam_path_reg[0] = '"';
	DWORD steam_path_size = sizeof( steam_path_reg ) - sizeof( char );

	if ( RegQueryValueExA( h_key, "SteamExe", nullptr, nullptr, (LPBYTE)( steam_path_reg + 1 ), &steam_path_size ) != ERROR_SUCCESS )
	{
		RegCloseKey( h_key );

		std::cout << "failed to get steam path" << std::endl;
		return EXIT_FAILURE;
	}
	RegCloseKey( h_key );

	auto steam_path = std::string( steam_path_reg ) + "\"";
	std::cout << "steam path - " << steam_path << std::endl;

	do
	{
		memory::kill_process( "csgo.exe" );
		memory::kill_process( "steam.exe" );
		memory::kill_process( "steamservice.exe" );
		memory::kill_process( "steamwebhelper.exe" );

		if ( !memory::is_process_open( "steam.exe" ) && !memory::is_process_open( "steamservice.exe" ) && !memory::is_process_open( "steamwebhelper.exe" ) )
			break;

		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
	while ( true );

	PROCESS_INFORMATION pi;
	if ( !memory::open_process( steam_path, { "-console" }, pi ) )
	{
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );

		std::cout << "failed to open steam.exe" << std::endl;
		return EXIT_FAILURE;
	}

	if ( memory::is_process_open( "steamservice.exe" ) )
	{
		std::cout << "steamservice is running, steam did not run as admin" << std::endl;
		return EXIT_FAILURE;
	}

	if ( !std::filesystem::exists( "vac3_inhibitor.dll" ) )
	{
		std::cout << "vac3_inhibitor.dll not found" << std::endl;
		return EXIT_FAILURE;
	}

	auto dll_path = std::filesystem::absolute( "vac3_inhibitor.dll" );
	std::cout << "dll path - " << dll_path << std::endl;

	auto loadlib_addrs = (LPVOID)GetProcAddress( GetModuleHandle( "kernel32.dll" ), "LoadLibraryA" );
	auto remote_path = VirtualAllocEx( pi.hProcess, NULL, dll_path.string().length(), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );

	WriteProcessMemory( pi.hProcess, remote_path, dll_path.string().c_str(), dll_path.string().length(), NULL );
	CreateRemoteThread( pi.hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)loadlib_addrs, (LPVOID)remote_path, NULL, NULL );

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	std::cout << "ready" << std::endl;

	Beep( 500, 100 );
	return EXIT_SUCCESS;
}
