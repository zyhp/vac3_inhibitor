#pragma once

namespace memory
{
	__forceinline std::uintptr_t tramp_hook( std::uintptr_t* src_addr, const std::uintptr_t dst_addr, const std::uintptr_t len )
	{
		auto* mem = VirtualAlloc( nullptr, len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );
		memcpy( mem, src_addr, len );

		const auto delta_addr = reinterpret_cast<std::uintptr_t>( src_addr ) - reinterpret_cast<std::uintptr_t>( mem ) - 5;

		//redirect to alloc'd memory
		*reinterpret_cast<char*>( reinterpret_cast<std::uintptr_t>( mem ) + len ) = static_cast<char>( 0xE9 );
		*reinterpret_cast<std::uintptr_t*>( reinterpret_cast<std::uintptr_t>( mem ) + len + 1 ) = delta_addr;

		DWORD dw_old_protect = 0;
		VirtualProtect( src_addr, len, PAGE_EXECUTE_READWRITE, &dw_old_protect );
		const auto delta_addr_mem = static_cast<std::uintptr_t>( dst_addr - reinterpret_cast<std::uintptr_t>( src_addr ) ) - 5;

		//redirect to actual function
		*src_addr = 0xE9;
		*reinterpret_cast<std::uintptr_t*>( reinterpret_cast<std::uintptr_t>( src_addr ) + 1 ) = delta_addr_mem;

		VirtualProtect( src_addr, len, dw_old_protect, &dw_old_protect );
		return reinterpret_cast<std::uintptr_t>( mem );
	}

	__forceinline std::uintptr_t relative_address( const std::uintptr_t addr, const size_t offs )
	{
		if ( !addr )
			return NULL;

		auto out = addr + offs;
		const auto ret = *reinterpret_cast<std::uint32_t*>( out );

		if ( !ret )
			return NULL;

		out = out + 4 + ret;
		return out;
	}

	inline std::uintptr_t get_size_of_nt( const IMAGE_NT_HEADERS* nt )
	{
		return offsetof( IMAGE_NT_HEADERS, OptionalHeader ) + nt->FileHeader.SizeOfOptionalHeader + nt->FileHeader.NumberOfSections * sizeof( IMAGE_SECTION_HEADER );
	}

	__forceinline std::uintptr_t hash_header( void* base )
	{
		auto* dos = static_cast<PIMAGE_DOS_HEADER>( base );
		if ( !dos )
			return 0;

		auto* const nt = reinterpret_cast<PIMAGE_NT_HEADERS32>( reinterpret_cast<std::uintptr_t>( dos ) + dos->e_lfanew );
		if ( nt->Signature != 0x4550 ) // compilation mode
			return 0;

		return crc32::hash( reinterpret_cast<PUCHAR>( nt ), get_size_of_nt( nt ), 0xFFFFFFFF );
	}

	inline std::uint8_t* pattern_scan_ida( HMODULE h_module, const char* signature )
	{
		static auto pattern_to_byte = []( const char* pattern )
		{
			auto bytes = std::vector<int>{};
			auto* const start = const_cast<char*>( pattern );
			auto* const end = const_cast<char*>( pattern ) + strlen( pattern );

			for ( auto* current = start; current < end; ++current )
			{
				if ( *current == '?' )
				{
					++current;
					if ( *current == '?' )
						++current;

					bytes.push_back( -1 );
				}
				else
					bytes.push_back( strtoul( current, &current, 16 ) );
			}
			return bytes;
		};

		auto* const dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>( h_module );
		auto* const nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>( reinterpret_cast<std::uint8_t*>( h_module ) + dos_header->e_lfanew );

		const auto size_of_image = nt_headers->OptionalHeader.SizeOfImage;
		auto* const scan_bytes = reinterpret_cast<std::uint8_t*>( h_module );

		auto pattern_bytes = pattern_to_byte( signature );

		const auto s = pattern_bytes.size();
		auto* const d = pattern_bytes.data();

		for ( auto i = 0ul; i < size_of_image - s; ++i )
		{
			auto found = true;
			for ( auto j = 0ul; j < s; ++j )
			{
				if ( scan_bytes[i + j] != d[j] && d[j] != -1 )
				{
					found = false;
					break;
				}
			}

			if ( found )
				return &scan_bytes[i];
		}
		return nullptr;
	}
}