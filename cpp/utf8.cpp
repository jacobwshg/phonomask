#include "utf8.h"
#include <cstddef>

std::vector<std::string_view>
Phmask::
utf8_str_to_cviews(const std::string &s)
{
    std::size_t slen {s.size()};
    std::vector<std::string_view> cviews {};
    for (std::size_t i {0}; i < slen; )
    {
        std::size_t clen {utf8_clen(s[i])};
        cviews.emplace_back(utf8_str_cview(s, i, clen));
        i += clen;
    }
    return cviews;
}

