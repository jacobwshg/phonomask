#ifndef PHMASK_UTF8_H
#define PHMASK_UTF8_H

#include <cstddef>
#include <bitset>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace Phmask
{
    inline std::size_t 
    utf8_clen(unsigned char byte0)
    {
        std::bitset<8> bs {byte0};
        if (!bs.test(7))
        {
            // 0xxx xxxx - single-byte utf-8 character
            return 1;
        }
        else if (!bs.test(6))
        {
            // 10xx xxxx - not initial byte in a utf-8 character
            return 0;
        }
        else if (!bs.test(5))
        {
            // 110x xxxx - initial byte in 2-byte character
            return 2;
        }
        else if (!bs.test(4))
        {
            // 1110 xxxx - initial byte in 3-byte character
            return 3;
        }
        else if (!bs.test(3))
        {
            // 1111 0xxx - initial byte in 4-byte character
            return 4;
        }
        throw std::runtime_error("Invalid utf-8 byte\n");
    }

    inline std::string_view
    utf8_str_cview(const std::string &s, std::size_t cidx, std::size_t clen)
    {
        std::string_view cv {s};
        cv.remove_prefix(cidx);
        cv.remove_suffix(s.size() - cidx - clen);
        return cv;
    }

    std::vector<std::string_view>
    utf8_str_to_cviews(const std::string &);
}

#endif

