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
        FeatOfsMaps(void) = default;
        ~FeatOfsMaps(void) = default;

        void populate(const std::vector<std::string>&);

        std::string &feature_at(const std::size_t);
        std::size_t offset_of(const std::string &);

        std::string str(void);
        std::string feature_layout_str(void);
    private:
        std::unordered_map<std::string, std::size_t> feat_ofs_map;
        std::vector<std::string> ofs_feat_map;
    };
}

#endif

