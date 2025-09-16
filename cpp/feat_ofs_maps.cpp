#include "feat_ofs_maps.h"
#include <vector>
#include <string>
#include <sstream>

//////////
#include <iostream>

Phmask::FeatOfsMaps::
FeatOfsMaps(std::vector<std::string>& header_row_fields) :
    feat_ofs_map {}, ofs_feat_map {}
{
    for (std::size_t colno {0}; colno < header_row_fields.size();
             ++colno)
    {
        if (colno == 0)
        {
            continue;
        }

        std::string &feature = header_row_fields[colno];
        std::size_t ofs = colno - 1;
        feat_ofs_map[feature] = ofs;
        ofs_feat_map.emplace_back(feature);
    }
}

std::string
Phmask::FeatOfsMaps::feature_layout_str(void)
{
    std::ostringstream lay_strm {};
    std::size_t nfeats = ofs_feat_map.size();

    for (std::size_t i {0}; i < nfeats; ++i)
    {
        lay_strm << ofs_feat_map[nfeats - 1 - i];
        if (i < nfeats - 1)
        {
            lay_strm << " | ";
        }
    }
    return lay_strm.str();
}

