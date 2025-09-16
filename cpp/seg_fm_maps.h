#include "feat_mtx.h"
#include <unordered_map>
#include <bitset>

namespace Phmask
{
    class SegFMMaps 
    {
    public:
        SegFMMaps(std::istream &);
    private:
        std::unordered_map<std::string, feat_mtx_t> seg_fm_map;
        std::unordered_map<feat_mtx_t, std::string, std::hash<feat_mtx_t>> 
            fm_seg_map;
    };
}

