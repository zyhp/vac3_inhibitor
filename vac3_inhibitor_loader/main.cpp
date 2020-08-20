#include "stdafx.hpp"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nShowCmd )
{
	std::vector<std::uint8_t> vec_bypass;
	if ( !util::read_file_to_memory( "vac3_inhibitor.dll", &vec_bypass ) )
	{
		_loge( "Failed to read the file." );
		return EXIT_FAILURE;
	}

	HKEY h_key = nullptr;
	if ( RegOpenKeyExA( HKEY_CURRENT_USER, "Software\\Valve\\Steam", 0, KEY_QUERY_VALUE, &h_key ) != ERROR_SUCCESS )
	{
		RegCloseKey( h_key );
		_loge( "Failed to open register." );
		return EXIT_FAILURE;
	}

	char ch_steam_path[ MAX_PATH ]; ch_steam_path[ 0 ] = '"';
	DWORD ui_steam_path_size = sizeof( ch_steam_path ) - sizeof( char );

	if ( RegQueryValueExA( h_key, "SteamExe", nullptr, nullptr, ( LPBYTE ) ( ch_steam_path + 1 ), &ui_steam_path_size ) != ERROR_SUCCESS )
	{
		RegCloseKey( h_key );
		_loge( "Steam path not found." );
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

		_loge( "Failed to open steam with the command line." );
		return EXIT_FAILURE;
	}

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	if ( memory::is_process_open( "steamservice.exe" ) || !memory::is_process_with_admin_rights( "steam.exe" ) )
	{
		_loge( "Steam is not open as admin." );
		return EXIT_FAILURE;
	}

	_logw( "Injecting..." );

	auto mod_callback = [ ]( blackbone::CallbackType type, void *, blackbone::Process &, const blackbone::ModuleData &modInfo )
	{
		std::string user32 = "user32.dll";
		if ( type == blackbone::PreCallback )
		{
			if ( modInfo.name == std::wstring( user32.begin( ), user32.end( ) ) )
				return blackbone::LoadData( blackbone::MT_Native, blackbone::Ldr_Ignore );
		}

		return blackbone::LoadData( blackbone::MT_Default, blackbone::Ldr_Ignore );
	};

	{
		blackbone::Process bb_steam_proc;
		bb_steam_proc.Attach( memory::get_process_id_by_name( "steam.exe" ), PROCESS_ALL_ACCESS );

		bb_steam_proc.Suspend( );

		if ( !bb_steam_proc.mmap( ).MapImage( vec_bypass.size( ), vec_bypass.data( ), false, blackbone::WipeHeader, mod_callback, nullptr, nullptr ).success( ) )
		{
			bb_steam_proc.Resume( );
			bb_steam_proc.Detach( );

			memory::kill_process( "steam.exe" );

			_loge( "Failed to inject into steam." );
			return EXIT_FAILURE;
		}

		bb_steam_proc.Resume( );
		bb_steam_proc.Detach( );
	}

	_logs( "Injected successfully, press enter to exit." );
	std::cin.get( );

	return EXIT_SUCCESS;
}