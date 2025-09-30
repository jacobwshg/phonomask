#ifndef FEAT_BDL_MASKS_H
#define FEAT_BDL_MASKS_H

#include "feat_mtx.h"
//#include "feature_profile.h"
#include <string_view>
#include <cstddef>
#include <string>
#include <sstream>

namespace Phmask
{
    //class FeatureProfile;

    struct FeatureBundleMasks
    {
        feat_mtx_t sel_mask; // 1 for features present in bundle
        feat_mtx_t val_mask; // 1 if present feature is positive

        FeatureBundleMasks(feat_mtx_t smask = EMPTY_FEAT_MTX, 
                           feat_mtx_t vmask = EMPTY_FEAT_MTX) :
            sel_mask {smask}, val_mask {vmask}
        {
        }

        std::string
        str(void) const
        {
            std::ostringstream ms_sstrm {};
            ms_sstrm << "Selection mask: " << sel_mask << "\n";
            ms_sstrm << "Value mask: " << val_mask << "\n";
            return ms_sstrm.str();
        }

        FeatureBundleMasks &
        add_positive(const std::size_t feature_index)
        {
            sel_mask.set(feature_index);
            val_mask.set(feature_index);
            return *this;
        }

        FeatureBundleMasks &
        add_negative(const std::size_t feature_index)
        {
            sel_mask.set(feature_index);
            return *this;
        }

        // true if ORIGINAL matches masks' conditions
        bool
        test_fm(const Phmask::feat_mtx_t original) const
        {
            return (original & sel_mask) == val_mask;
        }

        // Modify ORIGINAL in place
        void
        set_fm(Phmask::feat_mtx_t &original) const
        {
            original &= ~sel_mask;
            original |= val_mask;
        }
    };

/*
    FeatureBundleMasks
    masks_from_segment(const Phmask::FeatureProfile &, const std::string_view);

    FeatureBundleMasks
    masks_from_feat_bundle_str(const Phmask::FeatureProfile &, const std::string_view);

    FeatureBundleMasks
    masks_from_rule_tok(const Phmask::FeatureProfile &, const std::string_view);
*/

}

#endif

