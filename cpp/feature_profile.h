#include "feat_ofs_maps.h"
#include "seg_fm_maps.h"
#include <cstddef>

namespace Phmask
{
    class FeatureProfile
    {
    public:
        FeatureProfile(std::string &);
    private:
        FeatOfsMaps feat_ofs_maps;
        SegFMMaps seg_fm_maps;
    }
}

