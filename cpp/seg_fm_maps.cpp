#include "seg_fm_maps.h"
#include "feat_mtx.h"
#include "utils.h"
#include <cstddef>
#include <string_view>

void
Phmask::
SegFMMaps::populate(std::istream &table_stream) 
{
    /* Assume that the feature table's header row
       had been consumed to construct FeatIdxMaps
     */

    while (!table_stream.eof())
    {
        std::vector<std::string> seg_entry_fields
        {
            Phmask::fields_from_row(table_stream)
        };
        if (seg_entry_fields.size() < 1)
        {
            return;
        }
        std::string &segment {seg_entry_fields[0]};
        feat_mtx_t feat_mtx {0};
        for (std::size_t colno {1}; colno < seg_entry_fields.size(); ++colno)
        {
            std::size_t idx {colno - 1};
            /* TODO: Assuming binary features for now;
               underspecification is lost */
            if (seg_entry_fields[colno] == "+")
            {
                feat_mtx.set(idx);
            }
        }
        seg_fm_map[segment] = feat_mtx;
        fm_seg_map[feat_mtx] = segment;
    }
}

Phmask::feat_mtx_t 
Phmask::
SegFMMaps::feat_mtx_of(const std::string_view segment) const
{
    return Phmask::map_find_const(seg_fm_map, 
                                  segment, 
                                  "Segment not found\n");
}

const std::string & 
Phmask::
SegFMMaps::segment_of(const Phmask::feat_mtx_t feat_mtx) const
{
    return Phmask::map_find_const(fm_seg_map, 
                                  feat_mtx, 
                                  "No known segment for feature matrix\n");
}

