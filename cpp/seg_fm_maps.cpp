#include "seg_fm_maps.h"
#include "feat_mtx.h"
#include "utils.h"
#include <iostream>
#include <cstddef>

Phmask::SegFMMaps::
SegFMMaps(std::istream &table_stream) :
    seg_fm_map {}, fm_seg_map {}
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

