#include "seg_fm_maps.h"
#include "feat_mtx.h"
#include "utils.h"
#include <cstddef>
#include <string_view>
#include <sstream>

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
        this->seg_fm_map[segment] = feat_mtx;
        this->fm_seg_map[feat_mtx] = segment;
    }
}

Phmask::SegFMMaps &
Phmask::
SegFMMaps::add(const std::string &segment, const Phmask::feat_mtx_t &feat_mtx)
{
    this->seg_fm_map.try_emplace(segment, feat_mtx);
    this->fm_seg_map.try_emplace(feat_mtx, segment);
    return *this;
}

Phmask::SegFMMaps &
Phmask::
SegFMMaps::add_seg_alias(const std::string &alias, const std::string &original)
{
    const auto &it {this->seg_fm_map.find(original)};
    if (it != this->seg_fm_map.end())
    // Segment being aliased indeed exists
    {
        this->seg_fm_map.try_emplace(alias, it->second);
    }
    return *this;
}


Phmask::feat_mtx_t 
Phmask::
SegFMMaps::feat_mtx_of(const std::string_view segment) const
{
    std::ostringstream e_sstrm {};
    e_sstrm << "Segment [" << segment << "] not found\n";
    return Phmask::map_find_const(this->seg_fm_map, 
                                  segment, 
                                  e_sstrm.str());
}

const std::string & 
Phmask::
SegFMMaps::segment_of(const Phmask::feat_mtx_t feat_mtx) const
{
    return Phmask::map_find_const(this->fm_seg_map, 
                                  feat_mtx, 
                                  "No known segment for feature matrix\n");
}

std::string
Phmask::
SegFMMaps::str(void) const
{
    std::ostringstream sstrm {};
    for (const auto &[segment, feat_mtx] : this->seg_fm_map)
    {
        sstrm << segment << "\t" << feat_mtx << "\n";
    }
    return sstrm.str();
}

