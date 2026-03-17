#ifndef PHMASK_WORD_H
#define PHMASK_WORD_H

#include "feat_mtx.h"
#include "rule.h"
#include <vector>
#include <string>
#include <cstddef>
//#include <string_view>

namespace Phmask
{
	std::vector<std::string> word_to_segments(const std::string &);

	struct WordRepr;

	// Segment representation in word
	struct SegRepr
	{
		feat_mtx_t feat_mtx;

		/* Feature matrix of segment to insert before the 
			current position (as necessary) after applying 
			an insertion rule */
		feat_mtx_t insert_before_fm { 0UL };

		bool isnull( std::size_t null_bit ) const
		{
			return this->feat_mtx.test( null_bit );
		}

		bool issb( std::size_t sb_bit ) const
		{
			return this->feat_mtx.test( sb_bit );
		}
	};

	// Word representation
	struct WordRepr
	{
		// "Inherited" from the managing feature profile
		std::size_t null_bit;
		std::size_t wb_bit;
		std::size_t sb_bit;

		// true if insertion or deletion has effect
		bool isdirty { false };

		std::vector<SegRepr> segreprs;

		// An element is set to true if the corresponding
		// segment in the word will be affected by a rule
		std::vector<bool> apply_at;

		/* Commit insertion or deletion */
		void housekeep( void );

		WordRepr &
		apply_rule( const Phmask::Rule & );
	};
}

#endif

