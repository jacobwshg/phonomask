#ifndef PHMASK_RULE_H
#define PHMASK_RULE_H

#include "masks.h"
#include <vector>
#include <string>
#include <string_view>

namespace Phmask
{
	/*
		Preprocess a rule string.
		Example: "X -> Y / A1 A2 _ B1 B2" becomes 
		{ "X", "->", "Y", "/", "A1", "A2", "_", "B1", "B2" }
	*/
	std::vector<std::string_view>
	rule_str_toks( const std::string &rule_str );

	/*
		Parse feature-value pairs out of a feature bundle string.
		Example: "[+cons, -nas]" becomes { "+cons", "-nas" }
	*/
	std::vector<std::string_view>
	parse_feature_bundle_str( const std::string_view );

	struct RuleElem
	{
		FeatureBundleMasks masks;

		bool isnull( std::size_t null_bit ) const
		{
			return this->masks.val_mask.test( null_bit );
		}

		bool issb( std::size_t sb_bit ) const
		{
			return this->masks.val_mask.test( sb_bit );
		}
	};

	struct Rule
	{
		std::size_t null_bit;
		std::size_t wb_bit;
		std::size_t sb_bit; 

		RuleElem A;
		RuleElem B;
		std::vector<RuleElem> X;
		std::vector<RuleElem> Y;

		std::string masks_str( void ) const;
	};
}

#endif

