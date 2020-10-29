#include "stdafx.hpp"
#include "hooks.hpp"

bool hooks::initialize()
{
	auto* const h_steamservice = GetModuleHandleA( "steamservice.dll" );

	// LOADING
	{
		// E8 ? ? ? ? 84 C0 75 16 8B 43
		// memory::relative_address((std::uintptr_t)dw_loading, 1)

		auto* const dw_loading = memory::pattern_scan_ida( h_steamservice, "55 8B EC 83 EC 28 53 56 8B 75 08 8B" );
		if ( !dw_loading )
		{
			valve::msg( "[ hooks::initialize ] dw_loading is nullptr...\n" );
			return false;
		}

		o_loading = reinterpret_cast<loading_t>( memory::tramp_hook( reinterpret_cast<std::uintptr_t*>( dw_loading ),
												 reinterpret_cast<std::uintptr_t>( hk_loading ), 6 ) );
	}

	// CALLING
	{
		// E8 ? ? ? ? E8 ? ? ? ? 6A 00 FF 76 58
		// memory::relative_address((std::uintptr_t)dw_calling, 1)

		auto* const dw_calling = memory::pattern_scan_ida( h_steamservice, "55 8B EC 6A FF 68 ? ? ? ? 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC 6C 53 56" );
		if ( !dw_calling )
		{
			valve::msg( "[ hooks::initialize ] dw_calling is nullptr...\n" );
			return false;
		}

		o_calling = reinterpret_cast<calling_t>( memory::tramp_hook( reinterpret_cast<std::uintptr_t*>( dw_calling ),
												 reinterpret_cast<std::uintptr_t>( hk_calling ), 5 ) );
	}

	valve::msg( "[ hooks::initialize ] ready...\n" );
	return true;
}

bool __stdcall hooks::hk_loading( valve::vac_buffer* h_mod, char injection_flags )
{
	const auto nt_header_crc32 = memory::hash_header( h_mod->m_pRawModule );
	const auto b_ret = o_loading( h_mod, injection_flags );

	if ( nt_header_crc32 )
	{
		valve::msg( "[ VAC3 ] loading module crc32 [ 0x%.8X ].\n", nt_header_crc32 );

		// whitelisting these 2, these are responsable for connection, if we block the load it will kick us.
		if ( nt_header_crc32 == 0xCC29049A || nt_header_crc32 == 0x2B8DD987 )
			valve::uid_whitelist.push_back( h_mod->m_unCRC32 );
	}

	if ( h_mod->m_unCRC32 && std::find( valve::uid_whitelist.begin(), valve::uid_whitelist.end(), h_mod->m_unCRC32 ) != valve::uid_whitelist.end() )
	{
		valve::msg( "[ VAC3 ] uid [ 0x%.8X ] is whitelisted.\n", h_mod->m_unCRC32 );
		return b_ret;
	}

	if ( h_mod->m_pRunFunc )
	{
		h_mod->m_pRunFunc = nullptr;
		valve::msg( "[ VAC3 ] nulled _runfunc@20.\n" );
	}

	// do we know this shit?
	if ( nt_header_crc32 && std::find( valve::modules_hash.begin(), valve::modules_hash.end(), nt_header_crc32 ) == valve::modules_hash.end() )
	{
		// new module? dump and check if it needs to be loaded or not, gl & hf	
		valve::msg( "[ VAC3 ] unknown module loaded.\n" );

		// i will not update the modules list or reverse any that changed the crc, do by yourself.
		Beep( 500, 100 );
	}

	return b_ret;
}

int __fastcall hooks::hk_calling( void* ecx, void* edx, std::uint32_t crc_hash, char injection_mode, int unused_maybe, int runfunc_param1, int runfunc_param2, int runfunc_param3, int* runfunc_param4, int* region_or_size_check_maybe, int* module_status )
{
	// we don't care about this value
	o_calling( ecx, edx, crc_hash, injection_mode, unused_maybe, runfunc_param1, runfunc_param2, runfunc_param3, runfunc_param4, region_or_size_check_maybe, module_status );

	if ( *module_status != valve::SUCCESS && *module_status != valve::OTHER_SUCCESS )
	{
		*module_status = valve::SUCCESS;
		valve::msg( "[ VAC3 ] patched ret from uid [ 0x%.8X ].\n", crc_hash );
	}

	return 1;
}