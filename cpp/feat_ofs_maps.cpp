#include "feat_ofs_maps.h"
#include "svutils.h"
#include "masks.h"
#include "rule.h"
#include <vector>
#include <string>
#include <sstream>
#include <string_view>
#include <stdexcept>

void
Phmask::
FeatOfsMaps::populate(const std::vector<std::string>& header_row_fields) 
{
    for (std::size_t colno {0}; colno < header_row_fields.size();
             ++colno)
    {
        if (colno == 0)
        {
            continue;
        }

        const std::string &feature = header_row_fields[colno];
        std::size_t ofs = colno - 1;
        feat_ofs_map[feature] = ofs;
        ofs_feat_map.emplace_back(feature);
    }
}

std::string &
Phmask::
FeatOfsMaps::feature_at(const std::size_t offset)
{
    return ofs_feat_map.at(offset);
}

std::size_t 
Phmask::
FeatOfsMaps::offset_of(const std::string_view feature)
{
    const auto &it = feat_ofs_map.find(feature);
    if (it == feat_ofs_map.end())
    {
        throw std::runtime_error("Feature not found\n");
    }
    else
    {
        return it->second;
    }
}


std::string
Phmask::
FeatOfsMaps::str(void)
{
    std::ostringstream sstrm {};
    sstrm << "Offset\tFeature\n";
    for (std::size_t i {0}; i < ofs_feat_map.size(); ++i)
    {
        sstrm << i << "\t" << ofs_feat_map[i] << "\n";
    }
    return sstrm.str();
}

std::string
Phmask::
FeatOfsMaps::feature_layout_str(void)
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


Phmask::FeatureBundleMasks
Phmask::FeatOfsMaps::feat_bundle_str_to_masks
    (const std::string_view fb_str)
{
    Phmask::FeatureBundleMasks masks {};
    std::vector<std::string_view> fb_toks
    {
        Phmask::parse_feature_bundle_str(fb_str)
    };
    for (const std::string_view &tok : fb_toks)
    {
        std::size_t tok_len {tok.size()};
        std::string_view value {tok}, feature {tok};
        value.remove_suffix(tok_len - 1);
        feature.remove_prefix(1);

        std::size_t feature_offset = offset_of(feature);

        switch(value[0])
        {
        case '+':
            masks.add_positive(feature_offset);
            break;
        case '-':
            masks.add_negative(feature_offset);
            break;
        default:
            throw std::runtime_error("Feature bundle format not yet supported\n");
            break;
        }
    }
    return masks;
}

