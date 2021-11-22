#pragma once

namespace util
{
	inline std::string to_lower( std::string str )
	{
		std::transform( str.begin(), str.end(), str.begin(), static_cast<int( * )( int )>( ::tolower ) );
		return str;
	}
	inline std::string to_upper( std::string str )
	{
		std::transform( str.begin(), str.end(), str.begin(), static_cast<int( * )( int )>( ::toupper ) );
		return str;
	}
}