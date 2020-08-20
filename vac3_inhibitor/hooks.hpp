#pragma once

namespace hooks
{
	bool initialize( );

	using loading_t = bool( __stdcall * )( valve::vac_buffer *, char );
	inline loading_t o_loading;
	bool __stdcall hk_loading( valve::vac_buffer *, char );

	using calling_t = int( __fastcall * )( void *, void *, std::uint32_t, char, int, int, int, int, int *, int *, int * );
	inline calling_t o_calling;
	int __fastcall hk_calling( void *, void *, std::uint32_t, char, int, int, int, int, int *, int *, int * );
}