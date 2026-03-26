#include "rule.h"
#include "svutils.h"
#include <vector>
#include <string>
#include <string_view>
#include <cstddef>
#include <unordered_set>

//
#include <algorithm>
#include <iostream>

/*
 * @brief
 *   Preprocess a rule string into coarse-grained tokens.
 *   Example: "X -> Y / A1 A2 _ B1 B2" becomes 
 *   { "X", "->", "Y", "/", "A1", "A2", "_", "B1", "B2" }.
 * @param
 *   rule_str: a human-readable rule string.
 * @return
 *   temporary views into rule tokens, which will immediately be used to construct
 *   a rule object.
 */
std::vector<std::string_view>
Phmask::
rule_str_toks( const std::string &rule_str ) 
{
	std::vector<std::string_view> rule_toks {};
	const std::size_t r_len { rule_str.size() };

	std::string_view tok { "" };
	std::size_t 
		tok_begin { 0 }, // Initial pos in rule token
		tok_end   { 0 }; // Pos after rule token

	/* Whether current pos is within a feature bundle */
	bool in_feat_bdl { false };  

	for ( std::size_t i { 0 }; i < r_len; ++i )
	{
		const char c { rule_str[ i ] };
		switch ( c )
		{
		// TODO: use ICU to capture non-ascii arrows
		//case '-':
		//case '/':
		//case '_':
		case ' ':
			if ( !in_feat_bdl )
			{
				if ( tok_end > tok_begin )
				{ 
					tok = rule_str;
					tok.remove_prefix( tok_begin ); 
					tok.remove_suffix( r_len - tok_end );
					rule_toks.emplace_back( tok );
				}
				tok_end = ( tok_begin = i + 1 );
				continue;
			}
			[[fallthrough]];
		default:
			if ( c == '[' )
			{
				in_feat_bdl = true;
			}
			else if ( c == ']' )
			{
				in_feat_bdl = false;
			}
			tok_end = i + 1;
			break;
		}
	}
	if ( tok_end > tok_begin )
	{
		tok = rule_str; 
		tok.remove_prefix( tok_begin ); 
		tok.remove_suffix( r_len - tok_end );
		rule_toks.emplace_back( tok );
	}


	std::cout << "\n\nrule "<< rule_str << "tokens: \n";
	std::for_each(
		rule_toks.begin(),
		rule_toks.end(),
		[]( const std::string_view &tok )
		{
			std::cout << tok << "\n";
		}
	);
	std::cout<<"\n\n\n";

	return rule_toks;
}


/*
 * @brief
 *   Parse feature-value pairs out of a feature bundle string (within a rule string).
 *   Example: "[+cons, -nas]" becomes { "+cons", "-nas" }.
 * @param
 *   fb_str: temporary view into the feature bundle string.
 * @return
 *   temporary views into feature-value pairs as strings.
 */
std::vector<std::string_view>
Phmask::
parse_feature_bundle_str( const std::string_view fb_str )
{
	std::vector<std::string_view> toks {};
	const std::size_t fb_len { fb_str.size() };

	std::size_t
		tok_begin { 0 },
		tok_end   { 0 };

	for (std::size_t i { 0 }; i < fb_len; ++i)
	{
		const char c { fb_str[ i ] };
		switch ( c )
		{
		case '[':
		case ']':
		case ',':
		case ' ':
			if ( tok_end > tok_begin )
			{
				std::string_view tok { fb_str };
				tok.remove_prefix( tok_begin );
				tok.remove_suffix( fb_len - tok_end );
				toks.emplace_back( tok );
			}
			tok_end = ( tok_begin = i + 1 );
			break;
		default:
			tok_end = i + 1;
			break;
		}
	}
	return toks;
}

/*
 * @brief
 *   Constructs a debug string for bitmasks within a rule object,
 *   with A, B, X, Y component displayed separately.
 * @return
 *   the debug string.
 */
std::string
Phmask::
Rule::masks_str( void ) const
{
	std::string rule_str {};
	rule_str.reserve( 128 );
	rule_str +=
		std::string { "Rule\n" } 
		+ "A:\n" + this->A.masks.str()
		+ "B:\n" + this->B.masks.str()
		+ "X:\n";
	for ( const RuleElem &elem : this->X )
	{
		rule_str += elem.masks.str();
	}
	rule_str += "Y:\n";
	for ( const RuleElem &elem : this->Y )
	{
		rule_str += elem.masks.str();
	}
	return rule_str;
}

