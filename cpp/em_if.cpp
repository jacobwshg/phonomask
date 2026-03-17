
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

#include <fstream>
#include <iostream>

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

	std::string
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
std::string
PhmaskSession::populate( const std::string &table_str )
{
	std::istringstream table_sstrm { table_str };
	try
	{
		this->profile.populate( table_sstrm );
		return { "successfully populated feature profile" };
	}
	catch ( const std::runtime_error &e ) 
	{
		return { std::string{ "cpp error: " } + e.what() };
	}
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
	try
	{
		Phmask::Rule rule { this->profile.rule_from_str( rulestr ) };
		Phmask::WordRepr wordrepr { this->profile.wordrepr_from_str( wordstr ) };
		wordrepr.apply_rule( rule );
		return this->profile.wordrepr_to_str( wordrepr );
	}
	catch ( const std::runtime_error &e )
	{
		return { std::string{ "cpp error: " } + e.what() };
	}
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
	try
	{

		std::cout << "evaluating word\n\t"<<wordstr<<"\n"
			<< "on rules\n";
		std::for_each(
			rulestrs.begin(), rulestrs.end(),
			[]( const std::string &rs )
			{
				std::cout<<"\t"<<rs<<"\n";
			}
		);

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
			[ &rules, this ]( const std::string &r_str
		)
			{
				rules.emplace_back( this->profile.rule_from_str( r_str ) );
			}
		);

		Phmask::WordRepr wordrepr { this->profile.wordrepr_from_str( wordstr ) };

		std::for_each(
			rules.begin(),
			rules.end(),
			[ &wordrepr, &results, this ]( const Phmask::Rule &rule
		)
		{
				wordrepr.apply_rule( rule );
				std::string ws { this->profile.wordrepr_to_str( wordrepr ) };
				std::cout << "word after rule application: " << ws << "\n";
				results.emplace_back( std::move( ws ) );
			}
		);

		return results;
	}
	catch ( const std::runtime_error &e )
	{
		return { std::string{ "cpp error: " } + e.what() };
	}
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
	//emscripten::register_vector<std::string>( "StringVec" );

	emscripten::class_<PhmaskSession>( "PhmaskSession" )
		.constructor<>()
		.function( "populate", &PhmaskSession::populate )
		.function( "apply_rule", &PhmaskSession::apply_rule_to_word )
		.function( "apply_many", &PhmaskSession::apply_rules_to_word );
}
#endif

#ifndef __EMSCRIPTEN__
int
main(
	int argc, char *argv[]
)
{
	if ( argc<2 )
	{
		std::cout<<"Usage: phmask <table path>\n";
		return 2;
	}

	std::ifstream ifs { std::string { argv[1] } };
	PhmaskSession sess { };
	std::ostringstream oss {};
	oss << ifs.rdbuf();
	sess.populate( oss.str() );

	const std::string w { "atpark" };
	const std::vector<std::string> rs
	{
        "∅ -> a / [+cons] _ [+cons,]     ",
        "[+cons, -syl, -son, -voi] -> [+voi] / [-cons, +syl, +son, +voi] _ [-cons, +syl, +son, +voi]",
		" [-cont, +cons, ] -> ∅ / _ # "
	};

	sess.apply_rules_to_word( rs, w );
}
#endif

