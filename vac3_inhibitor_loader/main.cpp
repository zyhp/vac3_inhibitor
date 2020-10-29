#include "stdafx.hpp"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nShowCmd )
{
	HKEY h_key = nullptr;
	if ( RegOpenKeyExA( HKEY_CURRENT_USER, "Software\\Valve\\Steam", 0, KEY_QUERY_VALUE, &h_key ) != ERROR_SUCCESS )
	{
		RegCloseKey( h_key );
		return EXIT_FAILURE;
	}

	char ch_steam_path[ MAX_PATH ]; ch_steam_path[ 0 ] = '"';
	DWORD ui_steam_path_size = sizeof( ch_steam_path ) - sizeof( char );

	if ( RegQueryValueExA( h_key, "SteamExe", nullptr, nullptr, ( LPBYTE ) ( ch_steam_path + 1 ), &ui_steam_path_size ) != ERROR_SUCCESS )
	{
		RegCloseKey( h_key );
		return EXIT_FAILURE;
	}

	RegCloseKey( h_key );
	auto str_steam_path = std::string( ch_steam_path ) + "\"";

	do
	{
		memory::kill_process( "csgo.exe" );
		memory::kill_process( "steam.exe" );
		memory::kill_process( "steamservice.exe" );
		memory::kill_process( "steamwebhelper.exe" );

		if ( !memory::is_process_open( "steam.exe" ) && !memory::is_process_open( "steamservice.exe" ) && !memory::is_process_open( "steamwebhelper.exe" ) )
			break;

		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	} while ( true );

	PROCESS_INFORMATION pi;
	if ( !memory::open_process( str_steam_path, { "-console" }, pi ) )
	{
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );

		return EXIT_FAILURE;
	}

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	if ( memory::is_process_open( "steamservice.exe" ) || !memory::is_process_with_admin_rights( "steam.exe" ) )
		return EXIT_FAILURE;

	Beep( 500, 100 );
	return EXIT_SUCCESS;
}
