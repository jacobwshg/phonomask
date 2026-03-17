#include "utils.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string_view>

/*
 * @brief
 *   Obtain a pointer to the stream encapsulating a feature table file.
 * @param
 *   path: the path from which the table file is located and to be opened.
 * @return
 *   pointer to the file stream.
 */
std::unique_ptr<std::istream>
Phmask::
table_stream_ptr( const std::string &path )
{
	return std::make_unique<std::ifstream>( path );
}

/*
 * @brief
 *   Read a row from the table file stream, and parse it into a vector of fields.
 *   Allow reading for more fields than valid under the currently supported 
 *   max number of features; we'll truncate when adding fields to maps.
 */
std::vector<std::string>
Phmask::
fields_from_row( std::istream &is )
{
	std::vector<std::string> fields {};
	std::string field {};
	char c {};

	while ( is.get( c ) )
	{
		switch ( c )
		{
		case '\r':
			break;
		case '\n':
		case ',':
			fields.emplace_back( field );
			field.clear();
			if ( c == '\n' )
			{
				goto done;
			}
			break;
		default:
			field += c;
			break;
		}
	}

	done:
	if ( !field.empty() )
	{
		// EOF without trailing newline
		fields.emplace_back( field );
	}
	return fields;
}

/*
 * @brief
 *   Construct a native UTF-8 string from an ICU UTF-32 string.
 *   In the context of word parsing, each string is a single segment.
 */
std::string
Phmask::
unistr_to_str( const icu::UnicodeString &unistr )
{
	std::string s {};
	s.reserve(unistr.length());
	unistr.toUTF8String(s);
	return s;
}

