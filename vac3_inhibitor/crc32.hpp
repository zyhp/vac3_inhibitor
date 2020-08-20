#pragma once

namespace crc32
{
	static std::uint32_t crc32_table[ 8 ][ 256 ];
	__forceinline bool table( )
	{
		std::uint32_t i, j, crc;
		for ( i = 0; i < 256; i++ )
		{
			crc = i;
			crc = crc & 1 ? ( crc >> 1 ) ^ 0x82f63b78 : crc >> 1;
			crc = crc & 1 ? ( crc >> 1 ) ^ 0x82f63b78 : crc >> 1;
			crc = crc & 1 ? ( crc >> 1 ) ^ 0x82f63b78 : crc >> 1;
			crc = crc & 1 ? ( crc >> 1 ) ^ 0x82f63b78 : crc >> 1;
			crc = crc & 1 ? ( crc >> 1 ) ^ 0x82f63b78 : crc >> 1;
			crc = crc & 1 ? ( crc >> 1 ) ^ 0x82f63b78 : crc >> 1;
			crc = crc & 1 ? ( crc >> 1 ) ^ 0x82f63b78 : crc >> 1;
			crc = crc & 1 ? ( crc >> 1 ) ^ 0x82f63b78 : crc >> 1;
			crc32_table[ 0 ][ i ] = crc;
		}

		for ( i = 0; i < 256; i++ )
		{
			crc = crc32_table[ 0 ][ i ];
			for ( j = 1; j < 8; j++ )
			{
				crc = crc32_table[ 0 ][ crc & 0xff ] ^ ( crc >> 8 );
				crc32_table[ j ][ i ] = crc;
			}
		}

		return true;
	}

	inline std::uint32_t hash( const void *buf, size_t len, std::uint32_t init_crc )
	{
		static std::once_flag flag;
		std::call_once( flag, [ ]( )
		{
			table( );
		} );

		auto next = ( const char * ) buf;
		std::uint64_t crc = init_crc;

		if ( len == 0 )
			return ( std::uint32_t )crc;

		crc ^= 0xFFFFFFFF;

		while ( len && ( ( std::uintptr_t )next & 7 ) != 0 )
		{
			crc = crc32_table[ 0 ][ ( crc ^ *next++ ) & 0xff ] ^ ( crc >> 8 );
			len--;
		}

		while ( len >= 8 )
		{
			crc ^= *( std::uint64_t * )next;

			crc = crc32_table[ 7 ][ ( crc >> 0 ) & 0xff ] ^ crc32_table[ 6 ][ ( crc >> 8 ) & 0xff ]
				^ crc32_table[ 5 ][ ( crc >> 16 ) & 0xff ] ^ crc32_table[ 4 ][ ( crc >> 24 ) & 0xff ]
				^ crc32_table[ 3 ][ ( crc >> 32 ) & 0xff ] ^ crc32_table[ 2 ][ ( crc >> 40 ) & 0xff ]
				^ crc32_table[ 1 ][ ( crc >> 48 ) & 0xff ] ^ crc32_table[ 0 ][ ( crc >> 56 ) ];

			next += 8;
			len -= 8;
		}

		while ( len )
		{
			crc = crc32_table[ 0 ][ ( crc ^ *next++ ) & 0xff ] ^ ( crc >> 8 );
			len--;
		}

		return ( std::uint32_t )( crc ^= 0xFFFFFFFF );
	}
}