#ifndef FEAT_BDL_MASKS_H
#define FEAT_BDL_MASKS_H

#include "feat_mtx.h"
#include <string_view>
#include <cstddef>
#include <string>

namespace Phmask
{
    struct FeatureBundleMasks
    {
        feat_mtx_t sel_mask; // 1 for features present in bundle
        feat_mtx_t val_mask; // 1 if present feature is positive

        FeatureBundleMasks(feat_mtx_t = {0u}, feat_mtx_t = {0u});

        std::string str(void) const;

        inline void 
        add_positive(const std::size_t feature_offset)
        {
            sel_mask.set(feature_offset);
            val_mask.set(feature_offset);
        }

        inline void 
        add_negative(const std::size_t feature_offset)
        {
            sel_mask.set(feature_offset);
        }

        inline bool 
        test(const Phmask::feat_mtx_t original) const
        {
            return (original & sel_mask) == val_mask;
        }

        inline feat_mtx_t 
        set(const Phmask::feat_mtx_t original) const
        {
            return (original & ~sel_mask) | val_mask;
        }
    };

    FeatureBundleMasks
    masks_from_segment(const FeatureProfile &, const std::string_view);

    FeatureBundleMasks
    masks_from_feat_bundle_str(const FeatureProfile &, const std::string_view);

    FeatureBundleMasks
    masks_from_rule_tok(const FeatureProfile &, const std::string_view);
}

#endif

