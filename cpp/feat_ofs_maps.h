#ifndef FEAT_OFS_MAPS_H
#define FEAT_OFS_MAPS_H

#include <unordered_map>
#include <vector>
#include <string>
#include <cstddef>

namespace Phmask
{
    class FeatOfsMaps
    {
    private:
        std::unordered_map<std::string, std::size_t> feat_ofs_map;
        std::vector<std::string> ofs_feat_map;
    public:
        FeatOfsMaps(std::vector<std::string>&);
        std::string feature_layout_str(void);
    };
}

#endif

