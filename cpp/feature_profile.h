#ifndef FEATURE_PROFILE_H
#define FEATURE_PROFILE_H

#include "feat_ofs_maps.h"
#include "seg_fm_maps.h"
#include "feat_mtx.h"
#include <cstddef>
#include <string_view>

namespace Phmask
{
    class FeatureProfile
    {
    public:
        std::size_t num_feats;

        explicit FeatureProfile(const std::string &);
        ~FeatureProfile(void) = default;

        inline feat_mtx_t all_feats_mask(void);

        std::string seg_positive_feats_str(const std::string &);
        std::string seg_feat_mtx_str(const std::string &);

        FeatureBundleMasks segment_to_masks(std::string_view);
        FeatureBundleMasks 
        feat_bundle_str_to_masks(const std::string_view);

        FeatOfsMaps feat_ofs_maps;
        SegFMMaps seg_fm_maps;

    private:
        std::string 
        seg_effective_feats_str(const std::string &, Phmask::feat_mtx_t);
    };
}

#endif

