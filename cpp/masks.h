#ifndef FEAT_BDL_MASKS_H
#define FEAT_BDL_MASKS_H

#include "feat_mtx.h"
#include <string_view>
#include <cstddef>
#include <string>
#include <sstream>

namespace Phmask
{

    struct FeatureBundleMasks
    {
        feat_mtx_t sel_mask {EMPTY_FEAT_MTX}; // 1 for features present in bundle
        feat_mtx_t val_mask {EMPTY_FEAT_MTX}; // 1 if present feature is positive

        std::string
        str(void) const
        {
            std::ostringstream ms_sstrm {};
            ms_sstrm << "Selection mask: " << this->sel_mask << "\n";
            ms_sstrm << "Value mask: " << this->val_mask << "\n";
            return ms_sstrm.str();
        }

        /* Add a positive-valued feature */
        FeatureBundleMasks &
        add_positive(const std::size_t feature_index)
        {
            this->sel_mask.set(feature_index);
            this->val_mask.set(feature_index);
            return *this;
        }

        /* Add a negative-valued feature */
        FeatureBundleMasks &
        add_negative(const std::size_t feature_index)
        {
            this->sel_mask.set(feature_index);
            return *this;
        }

        /* Return true if ORIGINAL feature matrix 
           matches masks' conditions */
        bool
        test_fm(const Phmask::feat_mtx_t original) const
        {
            return (original & this->sel_mask) == this->val_mask;
        }

        /* Modify ORIGINAL feature matrix in place */
        void
        set_fm(Phmask::feat_mtx_t &original) const
        {
            original &= ~this->sel_mask;
            original |= this->val_mask;
        }
    };
}

#endif

