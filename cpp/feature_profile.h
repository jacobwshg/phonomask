#ifndef FEATURE_PROFILE_H
#define FEATURE_PROFILE_H

#include "feat_ofs_maps.h"
#include "seg_fm_maps.h"
#include "feat_mtx.h"
#include "rule.h"
#include <cstddef>
#include <string_view>

namespace Phmask
{
    class FeatureProfile
    {
    public:
        std::size_t num_feats;
        FeatOfsMaps feat_ofs_maps;
        SegFMMaps seg_fm_maps;

        explicit FeatureProfile(const std::string &);

        inline feat_mtx_t all_feats_mask(void) const;

        std::string seg_positive_feats_str(const std::string &) const;
        std::string seg_feat_mtx_str(const std::string &) const;

        FeatureBundleMasks segment_to_masks(std::string_view) const;
        FeatureBundleMasks 
        feat_bundle_str_to_masks(const std::string_view) const;

        FeatureBundleMasks
        rule_tok_to_masks(const std::string_view) const;

        Rule rule_from_str(const std::string &rule_str) const;

        std::string rule_masks_str(const Rule &);
    private:
        std::string 
        seg_effective_feats_str(const std::string &, Phmask::feat_mtx_t) const;
    };
}

#endif

