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

