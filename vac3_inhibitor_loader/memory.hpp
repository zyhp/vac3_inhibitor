#pragma once

namespace memory
{
	inline bool open_process( std::string str_path, std::vector<std::string> str_atrib, PROCESS_INFORMATION &proc )
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory( &si, sizeof( si ) );
		si.cb = sizeof( si );
		ZeroMemory( &pi, sizeof( pi ) );

		std::string wstr;
		wstr += str_path;

		for ( const auto &i : str_atrib )
			wstr += " " + i;

		auto b_ret = CreateProcessA( nullptr, const_cast< char * >( wstr.c_str( ) ), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi );

		proc = pi;
		return b_ret;
	}

	inline bool is_process_open( const std::string &str_proc )
	{
		if ( str_proc.empty( ) )
			return false;

		auto str_fl = str_proc;
		if ( str_fl.find_last_of( "." ) != std::string::npos )
			str_fl.erase( str_fl.find_last_of( "." ), std::string::npos );

		str_fl += ".exe";
		const auto handle = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, NULL );
		PROCESSENTRY32 m_entry; m_entry.dwSize = sizeof( m_entry );

		if ( !Process32First( handle, &m_entry ) )
			return false;

		do
		{
			if ( util::to_lower( m_entry.szExeFile ).compare( util::to_lower( str_fl ) ) == 0 )
			{
				const auto h_process = OpenProcess( PROCESS_VM_READ, false, m_entry.th32ProcessID );
				if ( h_process != nullptr )
					CloseHandle( h_process );

				CloseHandle( handle );
				return true;
			}
		} while ( Process32Next( handle, &m_entry ) );

		return false;
	}

	inline bool kill_process( const std::string &str_proc )
	{
		if ( str_proc.empty( ) )
			return false;

		auto str_fl = str_proc;
		if ( str_fl.find_last_of( "." ) != std::string::npos )
			str_fl.erase( str_fl.find_last_of( "." ), std::string::npos );

		str_fl += ".exe";
		const auto handle = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, NULL );
		PROCESSENTRY32 m_entry; m_entry.dwSize = sizeof( m_entry );

		if ( !Process32First( handle, &m_entry ) )
			return false;

		do
		{
			if ( util::to_lower( m_entry.szExeFile ).compare( util::to_lower( str_fl ) ) == 0 )
			{
				const auto h_process = OpenProcess( PROCESS_TERMINATE, false, m_entry.th32ProcessID );
				if ( h_process != nullptr )
				{
					TerminateProcess( h_process, 9 );
					CloseHandle( h_process );
				}

				CloseHandle( handle );
				return true;
			}
		} while ( Process32Next( handle, &m_entry ) );

		return false;
	}

	inline int get_process_id_by_name( const std::string &str_proc )
	{
		if ( str_proc.empty( ) )
			return false;

		auto str_fl = str_proc;
		if ( str_fl.find_last_of( "." ) != std::string::npos )
			str_fl.erase( str_fl.find_last_of( "." ), std::string::npos );

		str_fl += ".exe";

		const auto handle = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, NULL );
		PROCESSENTRY32 m_entry; m_entry.dwSize = sizeof( m_entry );

		if ( !Process32First( handle, &m_entry ) )
			return 0;

		do
		{
			if ( util::to_lower( m_entry.szExeFile ).compare( util::to_lower( str_fl ) ) == 0 )
			{
				CloseHandle( handle );
				return m_entry.th32ProcessID;
			}
		} while ( Process32Next( handle, &m_entry ) );

		return 0;
	}

	inline bool is_process_with_admin_rights( const std::string &str_proc = "" )
	{
		auto h_handle = GetCurrentProcess( );
		if ( !str_proc.empty( ) )
		{
			auto i_pid = get_process_id_by_name( str_proc );
			h_handle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, i_pid );
		}

		auto f_ret = FALSE;
		HANDLE h_token = nullptr;

		if ( OpenProcessToken( h_handle, TOKEN_QUERY, &h_token ) )
		{
			TOKEN_ELEVATION elevation;
			DWORD cb_size = sizeof( TOKEN_ELEVATION );
			if ( GetTokenInformation( h_token, TokenElevation, &elevation, sizeof( elevation ), &cb_size ) )
				f_ret = elevation.TokenIsElevated;
		}

		if ( h_token )
			CloseHandle( h_token );

		if ( h_handle )
			CloseHandle( h_handle );

		return f_ret;
	}
}