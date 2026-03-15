
#include "feature_profile.h"
#include "word.h"
#include "rule.h"
#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cstddef>
#include <algorithm>

namespace
{
	static Phmask::FeatureProfile PROFILE {};
}

/*
 * @brief
 *   Populate the feature profile, which will maintain internal mappings
 *   between IPA segments and bitmap-based feature representations,
 *   as well as mappings between feature names and their bitmap indices
 * @param
 *   table_str: a comma-separated string containing the contents of a
 *   feature table file: IPA segments are listed in the initial column,
 *   feature names are listed in the initial row, and each grid represents
 *   a segment's value with respect to a feature.
 */
void
profile_populate( std::string &table_str )
{
	std::istringstream table_sstrm { table_str };
	::PROFILE.populate( table_sstrm );
}

/*
 * @brief
 *   Parse a rule string and a word string, apply the rule to the word internally,
 *   and return the word after rule application.
 * @params
 *   rulestr: a human-readable rule description, such as "∅ -> a / [+cons] _ [+cons]".
 *   wordstr: a word consisting of IPA segments.
 * @return
 *   the word as IPA segments after applying the rule.
 *
 */
std::string
apply_rule_to_word(
	const std::string &rulestr,
	const std::string &wordstr
)
{
	Phmask::Rule rule { rule_from_string( rulestr ) };
	Phmask::WordRep wordrep { word_rep_from_str( wordstr ) };
	wordrep.apply_rule( rule );

	return ::PROFILE.wordrep_to_str( wordrep );
}

/*
 * @brief
 *   Apply a group of rules to a word in order, and return the state of the word
 *   after each step.
 * @param
 *   rulestrs: an ordered group of rules.
 *   wordstr: the IPA word upon which to apply rules.
 * @return
 *   the appearance of the IPA word after application of each rule.
 *
 */
std::vector<std::string>
apply_rules_to_word(
	const std::vector<std::string> &rulestrs,
	const std::string &wordstr
)
{
	const std::size_t rule_cnt { rulestrs.size(); }
	if ( !rule_cnt )
	{
		return {};
	}

	std::vector<std::string> results {};
	results.reserve( rule_cnt );

	std::vector<Phmask::Rule> rules {};
	rules.reserve( rule_cnt );

	std::for_each(
		rulestrs.begin(),
		rulestrs.end(),
		[]( const std::string &r_str )
		{
			rules.emplace_back( ::PROFILE.rule_from_str( r_str ) );
		}
	)

	Phmask::WordRep wordrep { ::PROFILE.word_rep_from_str( wordstr ) };

	std::for_each(
		rules.begin(),
		rules.end(),
		[ &wordrep ]( const Phmask::Rule &r )
		{
			wordrep.apply_rule( rule );
			results.emplace_back( ::PROFILE.word_rep_to_str( wordrep ) );
		}
	)

	return results;
}

