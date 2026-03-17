#ifndef PHMASK_UTILS_H
#define PHMASK_UTILS_H

#include <unicode/unistr.h>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <string_view>

namespace Phmask
{
	std::unique_ptr<std::istream>
	table_stream_ptr( const std::string & );

	std::vector<std::string>
	fields_from_row( std::istream & );

	std::string
	unistr_to_str( const icu::UnicodeString & );

	template<typename T_Map, typename T_Key>
	const typename T_Map::mapped_type &
	map_find_const(
		const T_Map &m, const T_Key &k, 
		const typename T_Map::mapped_type &default_
	)
	{
		const auto &it { m.find( k ) };
		return ( it == m.end() ) ? default_ : it->second;
	}
}

#endif

