#ifndef SEG_FM_MAPS_H
#define SEG_FM_MAPS_H

#include "feat_mtx.h"
#include "svutils.h"
#include <unordered_map>

namespace Phmask
{
    class SegFMMaps 
    {
    public:
        void populate(std::istream &);

        feat_mtx_t feat_mtx_of(const std::string_view) const;
        const std::string &segment_of(const feat_mtx_t) const;
    private:
        std::unordered_map<std::string, feat_mtx_t, SvStrHash, SvStrEq> seg_fm_map;
        std::unordered_map<feat_mtx_t, std::string, std::hash<feat_mtx_t>> 
            fm_seg_map;
    };
}

#endif

