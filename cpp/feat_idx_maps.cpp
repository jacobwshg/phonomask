#include "feat_idx_maps.h"
#include "utils.h"
#include <sstream>

void
Phmask::
FeatIdxMaps::populate(const std::vector<std::string>& header_row_fields) 
{
    for (std::size_t colno {0}; colno < header_row_fields.size();
             ++colno)
    {
        if (colno == 0)
        {
            continue;
        }

        const std::string &feature {header_row_fields[colno]};
        std::size_t idx {colno - 1};
        feat_idx_map[feature] = idx;
        idx_feat_map.emplace_back(feature);
    }
}

const std::string &
Phmask::
FeatIdxMaps::feature_at(const std::size_t index) const
{
    if (index < idx_feat_map.size())
    {
        return idx_feat_map[index];
    }
    else 
    {
        throw std::runtime_error("Not enough features\n");
    }
}

std::size_t 
Phmask::
FeatIdxMaps::index_of(const std::string_view feature) const
{
    return Phmask::map_find_const(feat_idx_map, 
                                  feature, 
                                  "Feature not found\n");
}

std::string
Phmask::
FeatIdxMaps::str(void) const
{
    std::ostringstream sstrm {};
    sstrm << "Index\tFeature\n";
    for (std::size_t i {0}; i < idx_feat_map.size(); ++i)
    {
        sstrm << i << "\t" << idx_feat_map[i] << "\n";
    }
    return sstrm.str();
}

std::string
Phmask::
FeatIdxMaps::feature_layout_str(void) const
{
    std::ostringstream lay_sstrm {};
    std::size_t nfeats {idx_feat_map.size()};

    for (std::size_t i {0}; i < nfeats; ++i)
    {
        lay_sstrm << idx_feat_map[nfeats - 1 - i];
        if (i < nfeats - 1)
        {
            lay_sstrm << " | ";
        }
    }
    return lay_sstrm.str();
}

