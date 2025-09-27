#include "masks.h"
#include "feat_mtx.h"
#include <cstddef>
#include <string>
#include <sstream>

Phmask::
FeatureBundleMasks::FeatureBundleMasks(Phmask::feat_mtx_t smask, 
                                       Phmask::feat_mtx_t vmask) :
    sel_mask {smask}, val_mask {vmask}
{
}

std::string
Phmask::
FeatureBundleMasks::str(void) const
{
    std::ostringstream ms_sstrm {};
    ms_sstrm << "Selection mask: " << sel_mask << "\n";
    ms_sstrm << "Value mask: " << val_mask << "\n";
    return ms_sstrm.str();
}

/*
Phmask::FeatureBundleMasks 
Phmask::
masks_from_segment(const Phmask::FeatureProfile &profile, 
                   const std::string_view segment)
{
    return 
        Phmask::FeatureBundleMasks 
        {
            profile.all_feats_mask(),
            profile.feat_mtx_of(segment)
        };
}

Phmask::FeatureBundleMasks
Phmask::
masks_from_feat_bundle_str(const Phmask::FeatureProfile &profile,
                           const std::string_view fb_str)
{
    FeatureBundleMasks masks {};
    std::vector<std::string_view> fb_toks
    {
        parse_feature_bundle_str(fb_str)
    };
    for (const std::string_view &tok : fb_toks)
    {
        std::size_t tok_len {tok.size()};
        std::string_view value {tok},  
                         feature {tok};
        value.remove_suffix(tok_len - 1);
        feature.remove_prefix(1);

        std::size_t feature_index {profile.index_of(feature)};

        switch(value[0])
        {
        case '+':
            masks.add_positive(feature_index);
            break;
        case '-':
            masks.add_negative(feature_index);
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
masks_from_rule_tok(const Phmask::FeatureProfile &profile,
                    const std::string_view r_tok)
{
    if (r_tok.find('[') != std::string::npos)
    {
        // Assume the token is a feature bundle
        return masks_from_feat_bundle_str(profile, r_tok);
    }
    else
    {
        return masks_from_segment(profile, r_tok);
    }
}
*/

