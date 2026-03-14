#include "feat_idx_maps.h"
#include "utils.h"
#include <sstream>

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
	for (
		std::size_t colno { 0 };
		colno < header_row_fields.size();
		++colno
	)
	{
		if ( colno == 0 )
		{
			continue;
		}

		const std::string &feature { header_row_fields[colno] };
		std::size_t idx { colno - 1 };
		this->feat_idx_map[ feature ] = idx;
		this->idx_feat_map.emplace_back( feature );
	}
}

/*
 * @brief
 *   Retrieve the feature name at a specified index.
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
		throw std::runtime_error( "Not enough features\n" );
	}
}

/*
 * @brief
 *   Retrieve the index of a specified feature.
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
		"Feature not found\n"
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
	std::ostringstream sstrm {};
	sstrm << "Index\tFeature\n";
	for ( std::size_t i { 0 }; i < this->idx_feat_map.size(); ++i )
	{
		sstrm << i << "\t" << this->idx_feat_map[ i ] << "\n";
	}
	return sstrm.str();
}

/*
 * @brief
 *   Construct the "layout" string of a feature-index map,
 *   which reflects how the feature names were displayed 
 *   in the header row of the original table file.
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
	std::ostringstream lay_sstrm {};
	std::size_t nfeats { this->idx_feat_map.size() };

	for ( std::size_t i { 0 }; i < nfeats; ++i )
	{
		lay_sstrm << this->idx_feat_map[ nfeats - 1 - i ];
		if ( i < nfeats - 1 )
		{
			lay_sstrm << " | ";
		}
	}
	return lay_sstrm.str();
}

