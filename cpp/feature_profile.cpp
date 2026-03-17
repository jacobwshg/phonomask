#include "feature_profile.h"
#include "feat_idx_maps.h"
#include "seg_fm_maps.h"
#include "feat_mtx.h"
#include "rule.h"
#include "masks.h"
#include "word.h"
#include "utils.h"
#include <vector>
#include <string>
#include <string_view>
#include <memory>
#include <array>
#include <cstddef>

/////////
#include <iostream>

/*
 * @brief
 *   Add reserved markers (null segment, word boundary, syllable boundary
 *   ASCII, and "significant" - see below) to the end of 
 *   segment-feature matrix maps. They will not be registered in 
 *   feature-index maps.
 */
void
Phmask::
FeatureProfile::add_reserved( void )
{
	// Add reserved symbols
	std::size_t 
		// 1 if null segment symbol
		null_bit   { this->num_feats },
		// 1 if word boundary symbol
		wb_bit     { null_bit + 1 },
		// 1 if syllable boundary symbol
		sb_bit     { wb_bit + 1 },
		// 1 if <$> or <.>, 0 if <ˈ> or <ˌ>
		sb_ascii_bit { sb_bit + 1 },
		// "significant" - 1 if <.> or <ˈ>, 0 if <$> or <ˌ>
		sb_sig_bit   { sb_ascii_bit + 1 };
	this->null_bit = null_bit;
	this->wb_bit = wb_bit;
	this->sb_bit = sb_bit;
	this->seg_fm_maps
		// null segment
		.add( "∅", feat_mtx_t{ 0UL }.set(null_bit) )
		// word boundary
		.add( "#", feat_mtx_t{ 0UL }.set(wb_bit) )
		// syllable boundary (rule)
		.add( "$", feat_mtx_t{ 0UL }.set(sb_bit).set(sb_ascii_bit) )
		// syllable boundary (data, unstressed)
		.add( ".", feat_mtx_t{ 0UL }.set(sb_bit).set(sb_ascii_bit).set(sb_sig_bit) )
		// syllable boundary (primary stress)
		.add( "ˈ", feat_mtx_t{ 0UL }.set(sb_bit).set(sb_sig_bit) )
		// syllable boundary (secondary stress)
		.add( "ˌ", feat_mtx_t{ 0UL }.set(sb_bit) );
}


/*
 * @brief
 *   Populate a feature profile using the content of a table encapsulated 
 *   in an input (string or file) stream.
 * @param
 *   table_strm: the input stream serving content of the table.
*/
void
Phmask::
FeatureProfile::populate( std::istream &table_strm )
{
	std::vector<std::string> header_row_fields
	{
		Phmask::fields_from_row( table_strm )
	};

	std::size_t num_cols { header_row_fields.size() };
	if ( num_cols > 1 )
	{
		this->num_feats = num_cols - 1;
	}
	if ( this->num_feats > MAX_NUM_FEATS )
	{
		std::cout << "Warning - currently supporting up to "
			<< MAX_NUM_FEATS 
			<< " features, additional features truncated\n";
		this->num_feats = MAX_NUM_FEATS;
	}

	this->feat_idx_maps.populate( header_row_fields );
	this->seg_fm_maps.populate( table_strm );

	this->add_reserved();
}

/*
 * @brief
 *   Construct a feature profile from a local csv table file.
 * @param
 *   path: the local path where the file is located.
*/
Phmask::
FeatureProfile::FeatureProfile( const std::string &path ):
	num_feats { 0 }, feat_idx_maps {}, seg_fm_maps {}, 
	null_bit {}, wb_bit {}, sb_bit {}
{
	std::unique_ptr<std::istream> table_sp 
	{ 
		Phmask::table_stream_ptr(path) 
	};
	std::istream &table_strm { *table_sp };

	this->populate( table_strm );
}

/*
 * @brief
 *   Retrieve the feature residing at a specified index in the
 *   feature-index map. Invokes the member map's relevant method.
 * @param
 *   index: the index to query.
 * @return
 *   the feature at `index`.
 *
 */
const std::string &
Phmask::
FeatureProfile::feature_at(const std::size_t index) const
{
	return this->feat_idx_maps.feature_at(index);
}

/*
 * @brief
 *   Retrieve the index of a specified feature from the feature-index map.
 *   Invokes the member map's relevant method.
 * @param
 *   feature: the name of the feature to query.
 * @return
 *   the feature's index.
 *
 */
std::size_t 
Phmask::
FeatureProfile::index_of( const std::string_view feature ) const
{
	return this->feat_idx_maps.index_of( feature );
}

/*
 * @brief
 *   Retrieve the feature matrix of a specified segment from the
 *   segment-feature matrix map. Invokes the member map's relevant method.
 * @param
 *   segment: the segment to query.
 * @return
 *   the segment's feature matrix.
 */  
Phmask::feat_mtx_t 
Phmask::
FeatureProfile::feat_mtx_of( const std::string_view segment ) const
{
	return this->seg_fm_maps.feat_mtx_of( segment );
}

/*
 * @brief
 *   Retrieve the segment described by a specified feature matrix from the
 *   segment-feature matrix map. Invokes the member map's relevant method.
 * @param
 *   feat_mtx: the feature matrix to query.
 * @return
 *   the segment.
 */  
const std::string &
Phmask::
FeatureProfile::segment_of( const feat_mtx_t feat_mtx ) const
{
	return this->seg_fm_maps.segment_of( feat_mtx );
}

/*
 * @brief
 *   Constructs a display string for a subset of a segment's features
 *   considered "effective" according to some filter, regardless of their 
 *   values.
 * @param
 *   segment: the IPA representation of the segment.
 * @param
 *   ef_mask: the effective feature filter. It has a set bit for each
 *   feature considered effective.
 * @return
 *   the display string of effective features of the segment, 
 *   in the format of "[ ..., +/-feat, ... ]"
 */
std::string
Phmask::
FeatureProfile::seg_effective_feats_str(
	const std::string &segment,
	Phmask::feat_mtx_t ef_mask
) const
{
	std::string ef_feats_str {};
	ef_feats_str.reserve( 128 );
	ef_feats_str += "[";

	const feat_mtx_t feat_mtx { this->feat_mtx_of( segment ) };

	if ( feat_mtx == INVALID_FEAT_MTX )
	{
		return std::string { "unknown segment\n" };
	}

	// print type if feat mtx belongs to reserved symbol
	if ( feat_mtx.test( this->null_bit ) )
	{
		ef_feats_str += "_null, ";
	}
	if ( feat_mtx.test( this->wb_bit ) )
	{
		ef_feats_str += "_word_boundary, ";
	}
	if ( feat_mtx.test( this->sb_bit ) )
	{
		ef_feats_str += "_syl_boundary, ";
	}

	// now, print features if feat mtx belong to valid segment
	for ( std::size_t idx { 0 }; idx < this->num_feats; ++idx )
	{
		if ( ef_mask.test( idx ) )
		// Feature at IDX is effective
		{
			ef_feats_str += ( feat_mtx.test( idx ) ? '+' : '-' );
			const std::string &feature { this->feature_at( idx ) };
			ef_feats_str += feature;
			ef_feats_str += ", ";
		}
	}
	ef_feats_str += "]";
	return ef_feats_str;
}

/*
 * @brief
 *   Construct a display string for all features of a segment and their values.
 *   This is a use case of `seg_effective_feats_str()`, with the effective
 *   mask simply being set for all valid features.
 * @param
 *   segment: the segment.
 * @return
 *   the display string for all of the segment's features.
 */
std::string
Phmask::
FeatureProfile::seg_feat_mtx_str( const std::string &segment ) const
{
	return this->seg_effective_feats_str(
		segment, 
		this->all_feats_mask()
	);
}

/*
 * @brief
 *   Construct a display string for all features in a segment whose values
 *   are positive.
 *   This is a use case of `seg_effective_feats_str()`, with the effective
 *   mask being the segment's own feature matrix. Since bits in the feature
 *   matrix represent the values of features, they naturally constitute a 
 *   filter for positive features in the segment.
 * @param
 *   segment: the segment.
 * @return
 *   the display string for all of the segment's positive features.
 */
std::string
Phmask::
FeatureProfile::seg_positive_feats_str( const std::string &segment ) const
{
	return this->seg_effective_feats_str(
		segment,
		this->feat_mtx_of( segment )
	);
}

/*
 * @brief
 *   Constructs a rule element from a segment or symbol literal appearing 
 *   in a rule string.
 *   In rule context, a segment or symbol is viewed as shorthand for a
 *   complete feature bundle, thus the selection mask has all bits set
 *   while the value mask is the segment or symbol's feature matrix.
 * @param
 *   segment: the base segment or symbol.
 * @return
 *   the constructed rule element.
 */
Phmask::RuleElem
Phmask::
FeatureProfile::segment_to_rule_elem( std::string_view segment ) const
{
	const feat_mtx_t seg_feat_mtx { this->feat_mtx_of( segment ) };

	// if the segment is not found, the returned feature matrix used as val_mask
	// will be INVALID_FEAT_MTX. In rule evaluation, no rule element should 
	// be able to match it with all features being selected by the flipped empty mtx.

	return 
		RuleElem
		{
			FeatureBundleMasks 
			{
				.sel_mask = Phmask::FLIPPED_EMPTY_FEAT_MTX,
				.val_mask = seg_feat_mtx
			}
		};
}

/*
 * @brief
 *   Constructs a rule element from a feature bundle string appearing in 
 *   a rule, which specifies any number of features and their values.
 * @params
 *   fb_str: the human-readable feature bundle string, such as
 *   "[+cons, -son]".
 * @return
 *   the constructed rule element.
 */
Phmask::RuleElem
Phmask::
FeatureProfile::feat_bundle_to_rule_elem( const std::string_view fb_str ) const
{
	FeatureBundleMasks masks {};
	std::vector<std::string_view> fb_toks
	{
		Phmask::parse_feature_bundle_str( fb_str )
	};
	for ( const std::string_view &tok : fb_toks )
	{
		std::size_t tok_len { tok.size() };
		std::string_view
			value { tok },  
			feature { tok };
		value.remove_suffix( tok_len - 1 );
		feature.remove_prefix( 1 );

		std::size_t i_feat { this->index_of( feature ) };
		if ( i_feat >= this->num_feats )
		{
			std::cout << "Warning - unknown feature " << feature << "\n";
		}

		// TODO: to support alpha or underspec, modify this part
		switch( value[0] )
		{
		case '+':
			masks.add_positive( i_feat );
			break;
		case '-':
			masks.add_negative( i_feat );
			break;
		default:
			std::cout << "Warning - value " << value[0] << " cast to binary negative\n";
			masks.add_negative( i_feat );
			break;
		}
	}
	return
		RuleElem { masks };
}

/*
 * @brief
 *   Constructs a rule element from a token in a rule string, by dispatching
 *   segments and feature bundles to their respective handlers.
 * @param
 *   tok: the rule token, which can be a segment or a feature bundle.
 * @return
 *   the constructed rule element.
 */
Phmask::RuleElem
Phmask::
FeatureProfile::rule_tok_to_elem( const std::string_view tok ) const
{
	if ( tok.find( '[' ) != std::string::npos )
	{
		// Assume the token is a feature bundle
		return this->feat_bundle_to_rule_elem(tok);
	}
	else
	{
		return this->segment_to_rule_elem(tok);
	}
}

/*
 * @brief
 *   Construct a complete rule from a rule string.
 * @param
 *   rule_str: the human-readable string describing the rule.
 * @return
 *   the constructed rule object.
 *
 */
Phmask::Rule
Phmask::
FeatureProfile::rule_from_str( const std::string &rule_str ) const
{
	static constexpr std::array<std::string_view, 3> 
		arrows
	{
		"→", "->", ">",
	};

	std::vector<std::string_view> rule_toks { Phmask::rule_str_toks( rule_str ) };

	enum class State
	{
		A, B, X, Y,
	};
	State parser_state { State::A };

	Rule rule {};
	rule.null_bit = this->null_bit;
	rule.wb_bit = this->wb_bit;
	rule.sb_bit = this->sb_bit;

	for ( std::string_view &tok : rule_toks )
	{
		if ( tok.size() < 1 )
		{
			continue;
		}
		bool isarrow { false };

		switch ( parser_state )
		{
		case State::A:
			for ( const std::string_view &arrow : arrows )
			{
				if ( tok == arrow )
				{
					parser_state = State::B;
					isarrow = true;
					break;
				}
			}
			if ( !isarrow )
			{
				std::cout << "state A token " << tok << "\n";

				rule.A = this->rule_tok_to_elem( tok );
			}
			break;
		case State::B:
			if ( tok[0] == '/' )
			{
				parser_state = State::X;
			}
			else
			{
				std::cout << "state B token " << tok << "\n";

				rule.B = this->rule_tok_to_elem( tok );
			}
			break;
		case State::X:
			if ( tok[0] == '_' )
			{
				parser_state = State::Y;
			}
			else
			{ 
				std::cout << "state X token " << tok << "\n";

				rule.X.emplace_back( this->rule_tok_to_elem( tok ) );
			}
			break;
		case State::Y:
			std::cout << "state Y token " << tok << "\n";

			rule.Y.emplace_back( this->rule_tok_to_elem( tok ) );
			break;
		default:
			break;
		}
	}

	return rule;
}

/*
 * @brief
 *   Construct a word representation from a data word string.
 *   This is done by segmenting the word and mapping the segments
 *   to segment representations wrapping their feature matrices.
 * @param
 *   word: a word string.
 * @return
 *   the word's representation.
 */
Phmask::WordRep
Phmask::
FeatureProfile::word_rep_from_str( const std::string &word ) const
{
	WordRep word_rep {};
	word_rep.null_bit = this->null_bit;
	word_rep.wb_bit = this->wb_bit;
	word_rep.sb_bit = this->sb_bit;

	std::vector<std::string> segments { Phmask::word_to_segments( word ) };
	std::size_t nsegs { segments.size() };
	word_rep.seg_reps.reserve( nsegs );
	word_rep.apply_at.resize( nsegs, false );

	for ( const std::string &segment : segments )
	{
		feat_mtx_t seg_feat_mtx { this->feat_mtx_of( segment ) };
		word_rep.seg_reps.emplace_back(
			SegRep
			{
				.feat_mtx = seg_feat_mtx,
				.insert_before_fm = EMPTY_FEAT_MTX,
			}
		);
	}

	return word_rep;
}

/* 
 * @brief
 *   Reconstruct a word representation back into a word string by recovering
 *   segments from their representations and concatenating them.
 * @param
 *   word_rep: a word representation.
 * @return
 *   the original word string, consisting of segments.
 */
std::string 
Phmask::
FeatureProfile::word_rep_to_str( const WordRep &word_rep ) const
{
	std::string word_str {};
	word_str.reserve( 32 );
	const std::vector<SegRep> &seg_reps { word_rep.seg_reps };
	for (std::size_t i { 0 }; i < seg_reps.size(); ++i )
	{
		const feat_mtx_t cur_fm { seg_reps[i].feat_mtx };
		const std::string &segment { this->segment_of( cur_fm ) };
		word_str += segment;
	}
	return word_str;
}

