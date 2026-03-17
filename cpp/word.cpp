#include "word.h"
#include "utils.h"
#include "feat_mtx.h"
#include <unicode/unistr.h>
#include <unicode/uchar.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>

////
#include <iostream>

namespace Phmask
{
	std::size_t
	update_pos(std::size_t, std::size_t, bool decr);

	bool
	try_rule_context(
		const WordRepr &, const Rule &, 
		std::size_t, std::size_t, 
		std::size_t, std::size_t
	);
}

/*
 * @brief
 *   Parse a data word into segments using the ICU library.
 *   
 */
std::vector<std::string> 
Phmask::
word_to_segments( const std::string &word )
{
	using UNISTR = icu::UnicodeString;

	if ( !word.size() )
	{
		// empty word
		return {};
	}

	UNISTR u_word { UNISTR::fromUTF8(word) };
	const std::int32_t uwlen { u_word.length() };

	/* Buffer for a single segment (which may be >= 1 Unicode chars) */
	UNISTR u_segbuf {};

	/* Vector of parsed segments, where each segment is a native string */
	std::vector<std::string> segments { };
	segments.reserve( uwlen + 2 );

	// if word does not begin with a word boundary, we add one by default
	// in case rules look for one
	if ( word[0] != '#' )
	{
		segments.emplace_back( "#" );
	}

	/* Worst case: each Unicode char in `u_word` is its own segment
	   (no ties, no modifiers), plus space for word boundary symbols */

	bool tied { false };

	for (std::int32_t i { 0 }; i < uwlen; ++i)
	{
		UChar32 c { u_word.char32At(i) };
		UCharCategory ccateg { U_CHAR_CATEGORY_COUNT };
		switch (c)
		{
		case U'(':
		case U')':
			// Don't support optional segments in data ( yet )
			break;
		case U'#':
			// Word boundary (maybe one data entry can span across words?)
		case U'ˈ':
		case U'ˌ':
			// Stressed syllable boundary
		case U'.':
			// (Unstressed) syllable boundary

			// Flush previous segment
			/*
			   If the following pair of statements were switched,
			   a symbol will incorrectly overwrite end of the previous segment 
			   and be flushed twice
			 */
			segments.emplace_back(unistr_to_str(u_segbuf));
			u_segbuf = c;
			break;
		case U'͡':
		case U'͜':
			tied = true;
			u_segbuf += c;
			break;
		default:
			ccateg = static_cast<UCharCategory>(u_charType(c));
			switch (ccateg)
			{
			/*
			   If a full character is encountered, in order for it to be 
			   recorded into a segment, it must be at the start of the segment 
			   or else be tied to the previous full character 
			   (plus possible marks and modifiers, which are simply 
			   appended to that character).
			   In all other cases, the full character belongs to the 
			   next segment; flush the currently recorded characters.
			   `U_LOWERCASE_LETTER` captures non-clicks and the bilabial 
			   clicks, and `U_OTHER_LETTER` captures the remaining clicks.
			 */
			case U_LOWERCASE_LETTER:
			case U_OTHER_LETTER:
				if (u_segbuf.isEmpty() || tied)
				{
					u_segbuf += c;
					tied = false;
				}
				else
				{
					segments.emplace_back(unistr_to_str(u_segbuf));
					u_segbuf = c; 
				}
				break;
			case U_NON_SPACING_MARK:
			case U_MODIFIER_LETTER:
				u_segbuf += c; 
				break;
			default:
				break;
			}
			break;
		}
	}
	if (!u_segbuf.isEmpty())
	{
		segments.emplace_back(unistr_to_str(u_segbuf));
	}
	/* Pad word end with word boundary symbol if it does not exist */
	if ( word[ word.size()-1 ] != '#' )
	{
		segments.emplace_back("#");
	}

	/*
	std::cout <<"word_to_segments(): ";
	for (std::size_t i{0};i<segments.size();++i){std::cout<<segments[i]<<" ";}
	std::cout<<"\n";
	*/

	return segments;
}

/*
 * @brief
 *   Helper for rule evaluation: Increment/decrement position for indexing a vector,
 *   safely overflowing to out-of-bounds position if out of range.
 *   If position already overflowed, don't change.
 * @param
 *   pos: index position.
 * @param
 *   endpos: overflow position, which may be set to the vector's size()
 *   for both incrementing and decrementing indices.
 * @param
 *   decr: whether indexing is in decrementing direction.
 * @return
 *   updated position.
 */
std::size_t
Phmask::
update_pos(
	std::size_t pos, std::size_t endpos,
	bool decr = false
)
{
	if (pos == endpos)
	{
		return endpos;
	}
	if (decr)
	{
		return pos == 0 ? endpos : --pos;
	}
	else
	{
		return pos == endpos ? pos : ++pos;
	}
}

/*
 * @brief
 *   Helper for rule evaluation: Resume scanning a word against a rule's conditions 
 *   from intermediate positions within WORD_REP and RULE's 
 *   X and Y parts.
 *   This function uses a recursive design to simplify skipping reserved symbols 
 *   and, in future implementations, possibly optional segments marked by parentheses
 *   in the rule string.
 */
bool
Phmask::
try_rule_context(
	const Phmask::WordRepr &wordrepr, const Phmask::Rule &rule,
	std::size_t wxpos, std::size_t wypos,
	std::size_t rxpos, std::size_t rypos
)
{

	const std::size_t
		null_bit { wordrepr.null_bit },
		sb_bit   { wordrepr.sb_bit },
		wb_bit   { wordrepr.wb_bit };

	bool rsvd_match { true };
	if ( rule.null_bit != null_bit )
	{
		std::cout
			<< "Word and rule null bit mismatch (should match): \n"
			<< "word: " << null_bit << ", rule: "<< rule.null_bit << "\n";
		rsvd_match = false;
	}
	if ( rule.sb_bit != sb_bit )
	{
		std::cout
			<< "Word and rule segment boundary bit mismatch (should match): \n"
			<< "word: " << sb_bit << ", rule: "<< rule.sb_bit << "\n";
		rsvd_match = false;
	}
	if ( rule.wb_bit != wb_bit )
	{
		std::cout
			<< "Word and rule word boundary bit mismatch (should match): \n"
			<< "word: " << wb_bit << ", rule: "<< rule.wb_bit << "\n";
		rsvd_match = false;
	}
	if ( !rsvd_match )
	{
		return false;
	}
 
	/* Obtain length of the word and the rule's X and Y sequences */
	const std::size_t
		wlen  { wordrepr.segreprs.size() }, 
		rxlen { rule.X.size() }, 
		rylen { rule.Y.size() };

	if ( ( rxpos >= rxlen ) && ( rypos >= rylen ) )
	/* Rule elements exhausted, all matching segments */
	{
		return true;
	}

	/* Trace through left of potential application site */
	if ( rxpos < rxlen )
	/* Element available in RULE's X */
	{
		if ( wxpos >= wlen )
		/* No remaining segment on the left in WORD_REP */
		{
			return false;
		}
		const SegRepr &wx { wordrepr.segreprs[wxpos] };
		const RuleElem &rx { rule.X[rxpos] };

		/* Word boundary test */
		const bool
			rx_iswb { rx.iswb( wb_bit ) },
			wx_iswb { wx.iswb( wb_bit ) };
		if ( wx_iswb ^ rx_iswb )
		{
			// for now, we require strict word boundary matching.
			// if one of rule or word has it and the other does not,
			// match fails.
			return false;
		}

		/* Segment boundary test */
		const bool 
			rx_issb { rx.issb( sb_bit ) },
			wx_issb { wx.issb( sb_bit ) };	
		if (
			rx.masks.test_fm( wx.feat_mtx )
			|| ( rx_issb && wx_issb )
		)
		/* Exact segment match, or syllable boundary match */
		{
			wxpos = update_pos( wxpos, wlen, true );
			rxpos = update_pos( rxpos, rxlen, true );
		}
		else if ( ( !rx_issb ) && wx_issb )
		/* Rule element doesn't specify syllable boundary
		 but "segment" (symbol in word) is syllable boundary 
		 - can skip?
		*/
		{
			wxpos = update_pos( wxpos, wlen, true );
		}
		else
		{
			return false;
		}
	}

	/* Then trace through right (if left all match) */
	if ( rypos < rylen )
	/* Element available in RULE's Y */
	{
		if ( wypos >= wlen )
		/* No remaining segment on the right in WORD_REP */
		{
			return false;
		}
		const SegRepr &wy { wordrepr.segreprs[wypos] };
		const RuleElem &ry { rule.Y[rypos] };

		/* Word boundary test */
		const bool
			ry_iswb { ry.iswb( wb_bit ) },
			wy_iswb { wy.iswb( wb_bit ) };
		if ( wy_iswb ^ ry_iswb )
		{
			// for now, we require strict word boundary matching.
			// if one of rule or word has it and the other does not,
			// match fails.
			return false;
		}

		const bool 
			ry_issb { ry.issb( sb_bit ) },
			wy_issb { wy.issb( sb_bit ) };
		if (
			ry.masks.test_fm( wy.feat_mtx )
			|| ( ry_issb && wy_issb )
		)
		/* Exact segment match, or syllable boundary match */
		{
			wypos = update_pos( wypos, wlen );
			rypos = update_pos( rypos, rylen );
		}
		else if ( ( !ry_issb ) && wy_issb )
		/* Rule element doesn't specify syllable boundary
		   but "segment" is syllable boundary (can skip) */
		{
			wypos = update_pos( wypos, wlen );
		}
		else
		{
			return false;
		}
	}

	// recurse with updated positions
	return try_rule_context( wordrepr, rule, wxpos, wypos, rxpos, rypos );
}


/* Commit insertions and deletions. */
void
Phmask::
WordRepr::housekeep( void )
{

	std::size_t cur_size { this->segreprs.size() };
	std::vector<SegRepr> segreprs_new {};

	if ( !this->isdirty )
	{
		goto done;
	}

	segreprs_new.reserve( cur_size );

	for ( std::size_t i { 0 }; i < cur_size; ++i )
	{
		const feat_mtx_t insert_fm { this->segreprs[i].insert_before_fm };
		if ( insert_fm.any() && !insert_fm.test( this->null_bit ) )
		/* Exists segment to be inserted before position I */
		{

			std::cout << "housekeep will insert empty feat mtx before position "<< i <<"\n";
			segreprs_new.emplace_back(
				SegRepr
				{
					insert_fm,
					EMPTY_FEAT_MTX,
				}
			);
			this->segreprs[i].insert_before_fm = EMPTY_FEAT_MTX;
		}
		if ( !this->segreprs[i].isnull( this->null_bit ) )
		/* Segment at position I not deleted */
		{
			segreprs_new.emplace_back( std::move( this->segreprs[i] ) );
		}
	}
	this->segreprs = std::move( segreprs_new );

	done:
		// reset word representation metadata across separate
		// rule applications
		this->isdirty = false;
		this->apply_at.assign( this->apply_at.size(), false );
}

Phmask::WordRepr &
Phmask::
WordRepr::apply_rule(
	const Phmask::Rule &rule
)
{

	//std::cout << "word rep null bit: "<<this->null_bit<<", rule null bit: "<<rule.null_bit<<"\n";

	/* Obtain length of the word and the rule's X and Y sequences */
	const std::size_t
		wlen  { this->segreprs.size() },
		rxlen { rule.X.size() },
		rylen { rule.Y.size() };

	/* Whether the rule can apply at any position at all */
	bool can_apply { false };

	/* True if the rule is insertion or deletion, 
	   respectively indicated by the A or B element
	   being the null segment symbol */
	const bool 
		isinsert { rule.A.isnull( rule.null_bit ) },
		isdelete { rule.B.isnull( rule.null_bit ) };

	if ( isinsert )
	{
		std::cout << "apply_rule: rule is insertion\n";
	}
	if ( isdelete )
	{
		std::cout << "apply_rule: rule is deletion\n";
	}

	/* Test each segment position for which X can fit on the left
	   and Y can fit on the right */
	for ( std::size_t pos { rxlen }; pos < wlen - rylen; ++pos )
	{
		const SegRepr &cur_seg { this->segreprs[ pos ] };
		const feat_mtx_t cur_fm { cur_seg.feat_mtx };

		if ( ( !rule.A.masks.test_fm( cur_fm ) ) && !isinsert )
		/* Current segment does not match A in rule,
		   and rule is not insertion; thus rule does not 
		   affect current segment */
		{
			std::cout << "site does not match rule at position "<<pos<<"\n";
			continue;
		}
	
		if ( !isinsert )
		{
			std::cout<< "\n\n\nmask test match\n";
			std::cout<<"rule A masks: \n";
			std::cout<<rule.A.masks.str();
			std::cout<<"cur feat mtx:\n";
			std::cout<<cur_fm.to_string()<<"\n\n\n\n";
		}

		/* Initialize starting positions in the word and rule 
		   to begin recursively matching X and Y elements to segments
		   outwards from the current segment (A)'s position */
		std::size_t 
			/* X in word: one position left of A */
			wxpos { pos > 0 ? pos - 1 : wlen },

			/* Y in word: same as A (unchanged if rule is insertion,
			   else advanced below) */
			wypos { pos },

			/* X in rule: rightmost position */
			rxpos { rxlen > 0 ? rxlen - 1 : rxlen },

			/* Y in rule: leftmost position */
			rypos { 0 };

		if ( !isinsert )
		/* Rule is not insertion; advance Y pos in word 
		   past current segment */
		{
			++wypos;
		}
		
		if (
			!try_rule_context( *this, rule, wxpos, wypos, rxpos, rypos )
		)
		/* Some adjacent segment does not match rule element;
		   rule cannot apply at current segment postion */
		{
			std::cout << "context does not match rule at position "<<pos<<"\n";

			continue;
		}

		/* Finally, all adjacent segments match the rule; 
		   mark current position */
		std::cout<<"can apply at position"<<pos<<"\n";

		this->apply_at[ pos ] = true;
		if (!can_apply)
		{
			can_apply = true;
		}
	}

	/* Apply rule at all eligible positions identified above */
	for ( std::size_t pos { 0 }; pos < wlen; ++pos )
	{
		SegRepr &cur_seg { this->segreprs[pos] };
		if ( !this->apply_at[pos] )
		{
			continue;
		}

		if ( isinsert )
		{
			/*
			 * Insertions are marked in `apply_at` before the positions
			 * where they take effect, thus set the position's insert_before_fm
			 * to schedule an insertion during housekeeping.
			 */
			rule.B.masks.set_fm( cur_seg.insert_before_fm );
		}
		else
		{
 			/* Update the current segment's feature matrix in-place.
			 * Includes deletion, in which case the B feat mtx is empty.
 			 */

			/*
			 * Important note: If a segment satisfies the rule's selection conditions
			 * but the feature matrix resulting from application does not map
			 * back to any know segment, should we cancel the application? 
			 */
			/*
			feat_mtx_t seg_fm_cp { cur_seg.feat_mtx };
			rule.B.masks.set_fm{ seg_fm_cp };
			if (
				// fm_seg_map.find( seg_fm_cp ) != fm_seg_map.end()
				// or FeatureProfile interface
			)
			{
				cur_seg.feat_mtx = seg_fm_cp;
			}
			*/

			rule.B.masks.set_fm( cur_seg.feat_mtx );
			if ( isdelete )
			{
				cur_seg.feat_mtx.set( this->null_bit );
			}
		}
	}

	if ( can_apply && ( isinsert || isdelete ) )
	{
		this->isdirty = true;
	}
	this->housekeep();

	return *this;
}

