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
    public:
        FeatOfsMaps(std::vector<std::string>&);
        std::string str(void);
        std::string feature_layout_str(void);
    private:
        std::unordered_map<std::string, std::size_t> feat_ofs_map;
        std::vector<std::string> ofs_feat_map;
    };
}

#endif

