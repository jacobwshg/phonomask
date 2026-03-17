
#include "feat_idx_maps.h"
#include "feat_mtx.h"
#include "utils.h"

/* @brief
 *   Populate a feature-index map with the header row of a feature table file.
 *   Column 0 is assumed to be empty, because it is directly above IPA segments.
 * @param
 *   header_row_fields: the header row as a vector. Example:
 *   { "", "cons", "voice", "cont" }. The mappings created will be 
 8   0-"cons", 1-"voice", 2-"cont".
 */ 
void
Phmask::
FeatIdxMaps::populate( const std::vector<std::string> &header_row_fields ) 
{
	this->idx_feat_map.clear();
	this->feat_idx_map.clear();

	std::size_t icol { 0 };

	if ( header_row_fields.empty() )
	{
		return;
	}

	const std::size_t num_feats = std::min(
		Phmask::MAX_NUM_FEATS,
		header_row_fields.size() - 1
	);
	this->idx_feat_map.reserve( num_feats );

	for ( const std::string &feature: header_row_fields )
	{
		std::cout << "constructing header row feature "<<feature<<"\n";

		if ( icol > num_feats )
		{
			break;
		}
		if ( icol == 0 )
		{
			// skip the empty cell atop col 0, which holds segments
			++icol;
			continue;
		}

		// feature at column 1 begins at idx 0
		const std::size_t idx { icol-1 };
		this->feat_idx_map[ feature ] = idx;
		this->idx_feat_map.emplace_back( feature );

		++icol;
	}
}

/*
 * @brief
 *   Retrieve the feature name at a specified index.
 *   If the index is out of bounds, return an error string.
 * @param
 *   index: the index.
 * @return
 *   the name of the feature.
 */
const std::string &
Phmask::
FeatIdxMaps::feature_at( const std::size_t index ) const
{
	if ( index < this->idx_feat_map.size() )
	{
		return this->idx_feat_map[index];
	}
	else 
	{
		static const std::string oob { "<index out of bounds>" };
		return oob;
	}
}

/*
 * @brief
 *   Retrieve the index of a specified feature.
 *   If the feature is not found, return an invalid idx.
 *   This index may be used by a reserved symbol flag, but not by any 
 *   valid feature.
 * @param
 *   feature: the name of the feature.
 * @return
 *   the feature's index.
 */
std::size_t 
Phmask::
FeatIdxMaps::index_of( const std::string_view feature ) const
{
	
	return Phmask::map_find_const(
		this->feat_idx_map, 
		feature, 
		Phmask::MAX_NUM_FEATS + 1
	);
}

/*
 * @brief
 *   Construct the display string for a feature-index map.
 * @return
 *   the feature-index map's string form. 
 *   Example: 
 *   ```
 *   0	cons
 *   1	voice
 *   2	cont
 *   ```
 */
std::string
Phmask::
FeatIdxMaps::str( void ) const
{
	std::string mp_str { };
	mp_str.reserve( 512 );
	mp_str += "Index\tFeature\n";
	for ( std::size_t i { 0 }; i < this->idx_feat_map.size(); ++i )
	{
		mp_str += std::to_string( i ) + "\t" + this->idx_feat_map[ i ] + "\n";
	}
	return mp_str;
}

/*
 * @brief
 *   Construct the "layout" string of a feature-index map, which reflects 
 *   how the feature names were displayed in the map (lowest index at RIGHT)
 *   as opposed to the original table file.
 * @return
 *   The feature-index map's layout string.
 *   Example:
 *   ```cons | voice | cont```
 *  
 */
std::string
Phmask::
FeatIdxMaps::feature_layout_str( void ) const
{
	const std::size_t num_feats { this->idx_feat_map.size() };
	if ( num_feats < 1 )
	{
		return { "" };
	}

	std::string lay_str {};
	lay_str.reserve( 256 );
	for ( std::size_t i { 0 }; i < num_feats; ++i )
	{
		lay_str += this->idx_feat_map[ num_feats - 1 - i ];
		if ( i + 1 < num_feats )
		{
			lay_str += " | ";
		}
	}
	return lay_str;
}

