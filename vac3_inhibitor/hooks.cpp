#include "stdafx.hpp"
#include "hooks.hpp"

bool hooks::initialize()
{
	auto* const h_steamservice = GetModuleHandleA( "steamservice.dll" );

	// LOADING
	{
		// 8B CF E8 ? ? ? ? 84 C0 75 0B + 2
		// memory::relative_address((std::uintptr_t)dw_loading, 1)

		auto* const dw_loading = memory::pattern_scan_ida( h_steamservice, "55 8B EC 83 EC 28 53 56 8B 75 08 8B" );
		if ( !dw_loading )
		{
			valve::msg( "[ VAC3 ] dw_loading is nullptr...\n" );
			return false;
		}

		o_loading = reinterpret_cast<loading_t>( memory::tramp_hook( reinterpret_cast<std::uintptr_t*>( dw_loading ),
												 reinterpret_cast<std::uintptr_t>( hk_loading ), 6 ) );
	}

	// CALLING
	{
		// E8 ? ? ? ? 89 86 ? ? ? ? E8 ? ? ? ? 6A 00	
		// memory::relative_address((std::uintptr_t)dw_calling, 1)

		auto* const dw_calling = memory::pattern_scan_ida( h_steamservice, "55 8B EC 6A FF 68 ? ? ? ? 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC 6C 53 56" );
		if ( !dw_calling )
		{
			valve::msg( "[ VAC3 ] dw_calling is nullptr...\n" );
			return false;
		}

		o_calling = reinterpret_cast<calling_t>( memory::tramp_hook( reinterpret_cast<std::uintptr_t*>( dw_calling ),
												 reinterpret_cast<std::uintptr_t>( hk_calling ), 5 ) );
	}

	valve::msg( "[ VAC3 ] ready...\n" );
	return true;
}

bool __stdcall hooks::hk_loading( valve::vac_buffer* h_mod, char injection_flags )
{
	const auto nt_header_crc32 = memory::hash_header( h_mod->m_pRawModule );
	const auto b_ret = o_loading( h_mod, injection_flags );

	if ( nt_header_crc32 )
	{
		valve::msg( "[ VAC3 ] loading module crc32 [ 0x%.8X ].\n", nt_header_crc32 );

		if ( nt_header_crc32 == 0xCC29049A || nt_header_crc32 == 0x2B8DD987 )
			valve::uid_whitelist.push_back( h_mod->m_unCRC32 );
	}

	if ( h_mod->m_unCRC32 && std::find( valve::uid_whitelist.begin(), valve::uid_whitelist.end(), h_mod->m_unCRC32 ) != valve::uid_whitelist.end() )
		return b_ret;

	if ( h_mod->m_pRunFunc )
		h_mod->m_pRunFunc = nullptr;

	return b_ret;
}

int __fastcall hooks::hk_calling( void* ecx, void* edx, std::uint32_t crc_hash, char injection_mode, int unused1, int id, int param1, int unused2, int param2, int param3, int* param4, int* size_check )
{
	auto status = o_calling( ecx, edx, crc_hash, injection_mode, unused1, id, param1, unused2, param2, param3, param4, size_check );

	if ( status != valve::SUCCESS && status != valve::OTHER_SUCCESS )
		status = valve::SUCCESS;

	return status;
}