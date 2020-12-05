#pragma once

namespace valve
{
	__forceinline void msg( const char* message, ... )
	{
		static auto* h_tier0 = GetModuleHandleA( "tier0_s.dll" );
		if ( !h_tier0 )
			return;

		using fn = void( __cdecl* )( const char* msg, va_list );
		static auto _msg = reinterpret_cast<fn>(GetProcAddress(h_tier0, "Msg"));
		if ( !_msg )
			return;

		char buf[1024];

		va_list list;
		va_start( list, message );
		vsprintf_s( buf, message, list );
		va_end( list );

		return _msg( buf, list );
	}

	enum vac_result_t
	{
		SUCCESS = 1,
		OTHER_SUCCESS = 2,
		ENTRY_POINT_FAIL = 16,
		MODULE_LOAD_FAILED = 22,
	};

	struct mapped_module
	{
		std::byte pad04[4];
		void* m_pModuleBase;
		PIMAGE_NT_HEADERS m_pNTHeaders;
	};

	struct vac_buffer
	{
		std::uint32_t m_unCRC32;
		std::byte pad04[4];
		mapped_module* m_pMappedModule;
		void* m_pRunFunc; // entrypoint
		int m_nLastResult;
		int m_nModuleSize;
		void* m_pRawModule;
	};

	inline std::vector<std::uintptr_t> uid_whitelist;
	inline std::vector<std::uintptr_t> modules_hash =
	{
		0x124b8c7f, 0x86bcebd0, 0x4f6c0697, 0xcc430292,
		0xdd7b857, 0x87cfbcbb, 0xd6ae10c8, 0xb5c9a7ff,
		0xcc29049a, 0x1da3ae2a, 0x2b8dd987, 0x54068fd1,
		0xd441f569, 0x6b23159d, 0x9cbf003e, 0xaefb39bf,
		0xd1b06964, 0x7ad3a803, 0xabe4f802, 0x3582a746,
		0x77ab4d78, 0x50594d78, 0x09dc4d78, 0x0a5a4d78,
		0x09294D78
	};
}
