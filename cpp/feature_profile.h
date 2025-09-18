#ifndef FEATURE_PROFILE_H
#define FEATURE_PROFILE_H

#include "feat_ofs_maps.h"
#include "seg_fm_maps.h"
#include <cstddef>

namespace Phmask
{
    class FeatureProfile
    {
    public:
        explicit FeatureProfile(const std::string &);
        ~FeatureProfile(void) = default;

        std::string seg_feat_mtx_str(const std::string &);
    private:
        std::size_t num_feats;
        FeatOfsMaps feat_ofs_maps;
        SegFMMaps seg_fm_maps;
    };
}

#endif

