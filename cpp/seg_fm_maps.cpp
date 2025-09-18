#include "seg_fm_maps.h"
#include "feat_mtx.h"
#include "utils.h"
#include "svutils.h"
#include <iostream>
#include <cstddef>
#include <stdexcept>
#include <string_view>

void
Phmask::
SegFMMaps::populate(std::istream &table_stream) 
{
    /* Assume that the feature table's header row
       had been consumed to construct FeatOfsMaps
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
        std::string &segment = seg_entry_fields[0];
        feat_mtx_t feat_mtx {0};
        for (std::size_t colno {1}; colno < seg_entry_fields.size(); ++colno)
        {
            std::size_t ofs {colno - 1};
            /* TODO: Assuming binary features for now;
               underspecification is lost */
            if (seg_entry_fields[colno] == "+")
            {
                feat_mtx.set(ofs);
            }
        }
        seg_fm_map[segment] = feat_mtx;
        fm_seg_map[feat_mtx] = segment;
    }
}

Phmask::feat_mtx_t 
Phmask::
SegFMMaps::feat_mtx_of(const std::string_view segment)
{
    const auto &it = seg_fm_map.find(segment);
    if (it == seg_fm_map.end())
    {
        throw std::runtime_error("Segment not found\n");
    }
    else 
    {
        return it->second;
    }
}

std::string & 
Phmask::
SegFMMaps::segment_of(const Phmask::feat_mtx_t feat_mtx)
{
    return fm_seg_map.at(feat_mtx);
}

