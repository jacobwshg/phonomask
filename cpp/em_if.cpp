
#include "feature_profile.h"
#include "word.h"
#include "rule.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>
#endif
#include <iostream>
#include <sstream>
#include <string>
#include <cstddef>
#include <algorithm>

namespace
{
	//static Phmask::FeatureProfile PROFILE {};
}

class PhmaskSession
{
private:
	Phmask::FeatureProfile profile {};

public:
	PhmaskSession( void );

	void
	populate( const std::string & );

	std::string
	apply_rule_to_word(
		const std::string &,
		const std::string &
	);

	std::vector<std::string>
	apply_rules_to_word(
		const std::vector<std::string> &,
		const std::string &
	);
};

/*
 * @brief
 *   Instantiate a session.
 */
PhmaskSession::PhmaskSession( void ):
	profile {}
{
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
PhmaskSession::populate( const std::string &table_str )
{
	std::istringstream table_sstrm { table_str };
	this->profile.populate( table_sstrm );
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
PhmaskSession::apply_rule_to_word(
	const std::string &rulestr,
	const std::string &wordstr
)
{
	Phmask::Rule rule { this->profile.rule_from_str( rulestr ) };
	Phmask::WordRep wordrep { this->profile.word_rep_from_str( wordstr ) };
	wordrep.apply_rule( rule );

	return this->profile.word_rep_to_str( wordrep );
}

/*
 * @brief
 *   Apply a group of rules to a word in order, and return the state of the word
 *   after each step.
 * @params
 *   rulestrs: an ordered group of rules.
 *   wordstr: the IPA word upon which to apply rules.
 * @return
 *   the appearance of the IPA word after application of each rule.
 *
 */
std::vector<std::string>
PhmaskSession::apply_rules_to_word(
	const std::vector<std::string> &rulestrs,
	const std::string &wordstr
)
{
	const std::size_t rule_cnt { rulestrs.size() };
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
		[ &rules, this ]( const std::string &r_str )
		{
			rules.emplace_back( this->profile.rule_from_str( r_str ) );
		}
	);

	Phmask::WordRep wordrep { this->profile.word_rep_from_str( wordstr ) };

	std::for_each(
		rules.begin(),
		rules.end(),
		[ &wordrep, &results, this ]( const Phmask::Rule &rule )
		{
			wordrep.apply_rule( rule );
			results.emplace_back( this->profile.word_rep_to_str( wordrep ) );
		}
	);

	return results;
}

#ifdef __EMSCRIPTEN__
/*
EMSCRIPTEN_BINDINGS( phmask )
{
	emscripten::function( "populateProfile", &profile_populate );
	emscripten::function( "applyRuleToWord", &apply_rule_to_word );
	emscripten::function( "applyRulesToWord", &apply_rules_to_word );
}
*/
EMSCRIPTEN_BINDINGS( phmask )
{
	emscripten::class_<PhmaskSession>( "PhmaskSession" )
		.constructor<>()
		.function( "populate", &PhmaskSession::populate )
		.function( "applyRule", &PhmaskSession::apply_rule_to_word )
		.function( "applyMany", &PhmaskSession::apply_rules_to_word );
}
#endif

int main()
{

}
