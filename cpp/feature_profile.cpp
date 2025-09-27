#include "feature_profile.h"
#include "feat_ofs_maps.h"
#include "seg_fm_maps.h"
#include "feat_mtx.h"
#include "rule.h"
#include "masks.h"
#include "utils.h"
#include <vector>
#include <string>
#include <string_view>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <unordered_set>

Phmask::
FeatureProfile::FeatureProfile(const std::string &path):
    num_feats {0}, feat_ofs_maps {}, seg_fm_maps {}
{
    std::unique_ptr<std::istream> table_sp 
    { 
        Phmask::table_stream_ptr(path) 
    };
    std::istream &table_strm {*table_sp};

    std::vector<std::string> header_row_fields
    {
        Phmask::fields_from_row(table_strm)
    };

    std::size_t num_cols {header_row_fields.size()};
    if (num_cols > 1)
    {
        num_feats = num_cols - 1;
    }

    feat_ofs_maps.populate(header_row_fields);
    seg_fm_maps.populate(table_strm);
}

std::string
Phmask::
FeatureProfile::seg_effective_feats_str(const std::string &segment, 
                                        Phmask::feat_mtx_t ef_mask) const
{
    std::string ef_feats_str {"["};

    Phmask::feat_mtx_t feat_mtx {seg_fm_maps.feat_mtx_of(segment)};
    for (std::size_t ofs {0}; ofs < num_feats; ++ofs)
    {
        if (ef_mask.test(ofs))
        // Feature at OFS is effective
        {
            ef_feats_str += (feat_mtx.test(ofs) ? "+" : "-");
            const std::string &feature {feat_ofs_maps.feature_at(ofs)};
            ef_feats_str += feature;
            ef_feats_str += ", ";
        }
    }
    ef_feats_str += "]";
    return ef_feats_str;
}

const std::string &
Phmask::
FeatureProfile::feature_at(const std::size_t offset) const
{
    return feat_ofs_maps.feature_at(offset);
}

std::size_t 
Phmask::
FeatureProfile::offset_of(const std::string_view feature) const
{
    return feat_ofs_maps.offset_of(feature);
}

Phmask::feat_mtx_t 
Phmask::
FeatureProfile::feat_mtx_of(const std::string_view segment) const
{
    return seg_fm_maps.feat_mtx_of(segment);
}

const std::string &
Phmask::
FeatureProfile::segment_of(const feat_mtx_t feat_mtx) const
{
    return seg_fm_maps.segment_of(feat_mtx);
}

std::string
Phmask::
FeatureProfile::seg_feat_mtx_str(const std::string &segment) const
{
    return seg_effective_feats_str(segment, all_feats_mask());
}

std::string
Phmask::
FeatureProfile::seg_positive_feats_str(const std::string &segment) const
{
    return seg_effective_feats_str(segment,
                                   seg_fm_maps.feat_mtx_of(segment));
}

/*
Phmask::FeatureBundleMasks 
Phmask::
FeatureProfile::segment_to_masks(std::string_view segment) const
{
    return 
        Phmask::FeatureBundleMasks 
        {
            all_feats_mask(),
            seg_fm_maps.feat_mtx_of(segment)
        };
}

Phmask::FeatureBundleMasks
Phmask::
FeatureProfile::feat_bundle_str_to_masks(const std::string_view fb_str) const
{
    Phmask::FeatureBundleMasks masks {};
    std::vector<std::string_view> fb_toks
    {
        Phmask::parse_feature_bundle_str(fb_str)
    };
    for (const std::string_view &tok : fb_toks)
    {
        std::size_t tok_len {tok.size()};
        std::string_view value {tok},  
                         feature {tok};
        value.remove_suffix(tok_len - 1);
        feature.remove_prefix(1);

        std::size_t feature_offset {feat_ofs_maps.offset_of(feature)};

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

Phmask::FeatureBundleMasks
Phmask::
FeatureProfile::rule_tok_to_masks(const std::string_view tok) const
{
    if (tok.find('[') != std::string::npos)
    {
        // Assume the token is a feature bundle
        return feat_bundle_str_to_masks(tok);
    }
    else
    {
        return segment_to_masks(tok);
    }
}

*/

/*
Phmask::Rule
Phmask::
FeatureProfile::rule_from_str(const std::string &rule_str) const
{
    return Rule {*this, rule_str};
}
*/

