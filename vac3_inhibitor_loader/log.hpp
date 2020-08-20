#pragma once

enum class msg_type_t : std::uint32_t
{
	LNONE = 0,
	LSUCCESS = 10,	/* green */
	LDEBUG = 11,	/* cyan */
	LWARN = 14,		/* yellow */
	LERROR = 12		/* red */
};

inline std::ostream &operator<< ( std::ostream &os, const msg_type_t type )
{
	switch ( type )
	{
		case msg_type_t::LSUCCESS:	return os << "+";
		case msg_type_t::LDEBUG:	return os << "~";
		case msg_type_t::LWARN:		return os << ">";
		case msg_type_t::LERROR:	return os << "!";
		default:					return os << "";
	};
}

class logger
{
private:
	std::shared_timed_mutex m {};

public:
	logger( )
	{
		FILE *conin, *conout;

		AllocConsole( );
		AttachConsole( GetCurrentProcessId( ) );

		SetConsoleTitleA( "[ vac3 inhibitor ]" );

		freopen_s( &conin, "conin$", "r", stdin );
		freopen_s( &conout, "conout$", "w", stdout );
		freopen_s( &conout, "conout$", "w", stderr );
	}

	~logger( )
	{
		const auto handle = FindWindowA( "ConsoleWindowClass", nullptr );
		ShowWindow( handle, SW_HIDE );
		FreeConsole( );
	}

	template< typename ... arg >
	void print( const msg_type_t type, const std::string &func, const std::string &format, arg ... a )
	{
		static auto *h_console = GetStdHandle( STD_OUTPUT_HANDLE );
		std::unique_lock<decltype( m )> lock( m );

		// parse and format the output
		const size_t size = 1 + std::snprintf( nullptr, 0, format.c_str( ), a ... );
		const std::unique_ptr<char[ ]> buf( new char[ size ] );
		std::snprintf( buf.get( ), size, format.c_str( ), a ... );
		const auto formated = std::string( buf.get( ), buf.get( ) + size - 1 );

		// print msg		
		if ( type != msg_type_t::LNONE )
		{
			SetConsoleTextAttribute( h_console, 15 /* white */ );
			std::cout << "[";

			SetConsoleTextAttribute( h_console, static_cast< WORD >( type ) );
			std::cout << type;

			SetConsoleTextAttribute( h_console, 15 /* white */ );
			std::cout << "] ";

			SetConsoleTextAttribute( h_console, 15 /* white */ );
			std::cout << "[ ";

			SetConsoleTextAttribute( h_console, static_cast< WORD >( type ) );
			std::cout << func;

			SetConsoleTextAttribute( h_console, 15 /* white */ );
			std::cout << " ] ";
		}

		if ( type == msg_type_t::LDEBUG )
			SetConsoleTextAttribute( h_console, 8 /* gray */ );
		else
			SetConsoleTextAttribute( h_console, 15 /* white */ );

		std::cout << formated << std::endl;
	}
};

#ifdef _DEBUG
inline auto g_logger = std::make_unique<logger>( );
#define _log(...) g_logger->print( msg_type_t::LNONE, __FUNCTION__, __VA_ARGS__ )
#define _logs(...) g_logger->print( msg_type_t::LSUCCESS, __FUNCTION__, __VA_ARGS__ )
#define _logd(...) g_logger->print( msg_type_t::LDEBUG, __FUNCTION__, __VA_ARGS__ )
#define _logw(...) g_logger->print( msg_type_t::LWARN, __FUNCTION__, __VA_ARGS__ )
#define _loge(...) g_logger->print( msg_type_t::LERROR, __FUNCTION__, __VA_ARGS__ )
#else
#define _log(...)
#define _logs(...)
#define _logd(...)
#define _logw(...)
#define _loge(...)
#endif