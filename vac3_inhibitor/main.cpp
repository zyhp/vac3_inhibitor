#include "stdafx.hpp"
#include "hooks.hpp"

DWORD __stdcall OnDllAttach( )
{
	while ( !GetModuleHandleA( "tier0_s.dll" ) )
		std::this_thread::sleep_for( std::chrono::milliseconds( 25 ) );

	while ( !GetModuleHandleA( "steamservice.dll" ) )
		std::this_thread::sleep_for( std::chrono::milliseconds( 25 ) );

	valve::msg( "[ OnDllAttach ] initializing...\n" );

	if ( !hooks::initialize( ) )
		exit( EXIT_FAILURE );

	while ( true )
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
}

BOOL __stdcall DllMain( HMODULE hModule, DWORD ulReasonForCall, LPVOID lpReserved )
{
	switch ( ulReasonForCall )
	{
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls( hModule );
			_beginthreadex( nullptr, 0, reinterpret_cast< unsigned( __stdcall * )( void * ) >( OnDllAttach ), nullptr, 0, nullptr );
			break;

		default:
			break;
	}
	return TRUE;
}