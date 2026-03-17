
#include "seg_fm_maps.h"
#include "feat_mtx.h"
#include "utils.h"
#include <cstddef>
#include <string_view>

void
Phmask::
SegFMMaps::populate( std::istream &table_stream ) 
{
	/* Assume that the feature table's header row
	 * had been consumed to construct FeatIdxMaps.
	 */

	this->seg_fm_map.clear();
	this->fm_seg_map.clear();

	while ( !table_stream.eof() )
	{
		std::vector<std::string> seg_entry_fields
		{
			Phmask::fields_from_row(table_stream)
		};
		if ( seg_entry_fields.size() < 1 )
		{
			return;
		}

		std::string segment { };
		feat_mtx_t feat_mtx { 0UL };

		const std::size_t num_feats
		{ 
			std::min( Phmask::MAX_NUM_FEATS, seg_entry_fields.size()-1 )
		};

		// idx is meant to be feature idx, so feature in col 1 has idx 0
		std::size_t idx { 0 };
		bool seen_segment { false };
		for ( const std::string &field : seg_entry_fields )
		{
			if ( !seen_segment )
			{
				// assign col 0 field to segment, don't increment idx
				segment = field;
				seen_segment = true;
				continue;
			}

			if ( idx < num_feats )
			{
				// field is feature value within supported feature length
				// TODO: support only binary features for now
				if ( field[ 0 ] == '+' )
				{
					feat_mtx.set( idx );
				}
			}
			else 
			{
				// truncate too long rows to supported length
				break;
			}
			++idx;
		}
		this->seg_fm_map[ segment ] = feat_mtx;
		this->fm_seg_map[ feat_mtx ] = segment;
	}
}

/*
 * @brief
 *   Add a pair of segment and feature matrix read from file to maps.
 */
Phmask::SegFMMaps &
Phmask::
SegFMMaps::add(const std::string &segment, const Phmask::feat_mtx_t &feat_mtx)
{
	this->seg_fm_map.try_emplace(segment, feat_mtx);
	this->fm_seg_map.try_emplace(feat_mtx, segment);
	return *this;
}

/*
 * @brief
 *   Add an alias for an existing segment, so that both can map to the same
 *   feature matrix. However, note that keying the feature matrix to segment map
 *   with the shared feature matrix can only return one of the segments.
 */
Phmask::SegFMMaps &
Phmask::
SegFMMaps::add_seg_alias( const std::string &alias, const std::string &original )
{
	const auto &mpit { this->seg_fm_map.find( original ) };
	if ( mpit != this->seg_fm_map.end() )
	/* Segment being aliased indeed exists */
	{
		this->seg_fm_map.try_emplace( alias, mpit->second );
	}
	return *this;
}

/*
 * @brief
 *   Retrieve the feature matrix of a segment. If the segment does not exist,
 *   return a invalid feature matrix.
 */
Phmask::feat_mtx_t 
Phmask::
SegFMMaps::feat_mtx_of( const std::string_view segment ) const
{
	return Phmask::map_find_const(
		this->seg_fm_map, 
		segment, 
		Phmask::INVALID_FEAT_MTX
	);
}

/*
 * @brief
 *   Retrieve the segment described by a feature matrix.
 *   If the feature matrix does not exist, an "unknown segment" symbol is returned.
 */
const std::string & 
Phmask::
SegFMMaps::segment_of( const Phmask::feat_mtx_t feat_mtx ) const
{
	static const std::string unknown_seg { "?" };
	return Phmask::map_find_const(
		this->fm_seg_map, 
		feat_mtx,
		unknown_seg
	);
}

/*
 * @brief
 *   Construct a debug string with segments and their feature matrices as bitmaps.
 */
std::string
Phmask::
SegFMMaps::str(void) const
{
	std::string mp_str {};
	mp_str.reserve( 2048 );
	for ( const auto &[segment, feat_mtx] : this->seg_fm_map )
	{
		mp_str +=
			segment +  "\t"  
			+ feat_mtx.to_string() + "\n";
	}
	return mp_str;
}

