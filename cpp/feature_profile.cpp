#include "feature_profile.h"
#include "feat_ofs_maps.h"
#include "seg_fm_maps.h"
#include <string>
#include <sstream>
#include <memory>

Phmask::FeatureProfile::
FeatureProfile(std::string &path):
    feat_ofs_maps {}, seg_fm_maps {}
{
    std::unique_ptr<std::istream> table_sp
    {
        table_stream_ptr<path>
    };
    std::istream &table_strm {*table_sp};
    std::vector{std::string} header_row_fields
    {
        fields_from_row(table_strm);
    }
    feat_ofs_maps.populate(header_row_fields);
    seg_fm_maps.populate(table_strm);
}

