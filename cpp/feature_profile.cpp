#include "feature_profile.h"
#include "feat_ofs_maps.h"
#include "seg_fm_maps.h"
#include "utils.h"
#include <string>
#include <sstream>
#include <memory>

Phmask::
FeatureProfile::FeatureProfile(std::string &path):
    num_feats {0}, feat_ofs_maps {}, seg_fm_maps {}
{
    std::unique_ptr<std::istream> table_sp 
    { 
        Phmask::table_stream_ptr(path) 
    };
    std::istream &table_strm {*table_sp};

    std::vector<std::string> header_row_fields
    {
        Phmask::fields_from_row(table_strm)
    };

    std::size_t num_cols = header_row_fields.size();
    if (num_cols > 1)
    {
        num_feats = num_cols - 1;
    }

    feat_ofs_maps.populate(header_row_fields);
    seg_fm_maps.populate(table_strm);
}

std::string 
Phmask::
FeatureProfile::seg_feat_mtx_str(std::string segment)
{
    std::ostringstream fm_sstrm {};
    fm_sstrm << "[";

    feat_mtx_t feat_mtx {seg_fm_maps.feat_mtx_of(segment)};

    for (std::size_t ofs = 0; ofs < num_feats; ++ofs)
    {
        std::string feature {feat_ofs_maps.feature_at(ofs)};
        fm_sstrm << (feat_mtx.test(ofs) ? "+" : "-");
        fm_sstrm << feature;
        if (ofs < num_feats - 1)
        {
            fm_sstrm << ", ";
        }
    }

    fm_sstrm << "]";
    return fm_sstrm.str();
}

