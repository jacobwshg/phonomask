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
        ~FeatureBundleMasks(void) = default;

        std::string str(void);

        //inline 
        void add_positive(const std::size_t);
        //inline 
        void add_negative(const std::size_t);

        //inline 
        bool test(const feat_mtx_t);
        //inline 
        feat_mtx_t set(const feat_mtx_t);
    };
}

#endif

