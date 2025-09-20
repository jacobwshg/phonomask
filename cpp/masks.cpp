#include "masks.h"
#include "feat_mtx.h"
#include <cstddef>
#include <string>
#include <sstream>

Phmask::FeatureBundleMasks::FeatureBundleMasks(Phmask::feat_mtx_t smask, 
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

//inline 
void
Phmask::
FeatureBundleMasks::add_positive(const std::size_t feature_offset)
{
    sel_mask.set(feature_offset);
    val_mask.set(feature_offset);
}

//inline 
void 
Phmask::
FeatureBundleMasks::add_negative(const std::size_t feature_offset)
{
    sel_mask.set(feature_offset);
}

//inline 
bool 
Phmask::
FeatureBundleMasks::test(const Phmask::feat_mtx_t original) const
{
    return (original & sel_mask) == val_mask;
}

//inline 
Phmask::feat_mtx_t 
Phmask::
FeatureBundleMasks::set(const Phmask::feat_mtx_t original) const
{
    return (original & ~sel_mask) | val_mask;
}

