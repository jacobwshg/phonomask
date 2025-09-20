#ifndef FEAT_OFS_MAPS_H
#define FEAT_OFS_MAPS_H

#include "svutils.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <cstddef>

namespace Phmask
{
    class FeatOfsMaps
    {
    public:
        void populate(const std::vector<std::string>&);

        const std::string &feature_at(const std::size_t) const;
        std::size_t offset_of(const std::string_view) const;

        std::string str(void) const;
        std::string feature_layout_str(void) const;

    private:
        std::unordered_map<std::string,
                           std::size_t,
                           SvStrHash,
                           SvStrEq> feat_ofs_map;
        std::vector<std::string> ofs_feat_map;
    };
}

#endif

