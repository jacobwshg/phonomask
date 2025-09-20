#ifndef PHMASK_UTILS_H
#define PHMASK_UTILS_H

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <string_view>

namespace Phmask
{
    std::unique_ptr<std::istream>
    table_stream_ptr(const std::string &);

    void
    fields_from_row(std::istream &, std::vector<std::string> &);

    std::vector<std::string>
    fields_from_row(std::istream &);

    template<typename T_Map, typename T_Key> const typename T_Map::mapped_type &
    map_find_const(const T_Map &m, const T_Key &k, const std::string &e_msg)
    {
        const auto &it = m.find(k);
        if (it == m.end())
        {
            throw std::runtime_error(e_msg);
        }
        else
        {
            return it->second;
        }
    }
}

#endif

