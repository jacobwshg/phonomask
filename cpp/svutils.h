#ifndef SVUTILS_H
#define SVUTILS_H

#include <cstddef>
#include <string>
#include <string_view>

struct SvStrHash
{
    using is_transparent = void;
    std::size_t operator()(const std::string_view) const;
    std::size_t operator()(const std::string &) const;
};

struct SvStrEq
{
    using is_transparent = void;
    bool operator()(const std::string_view, const std::string_view) const;
    bool operator()(const std::string_view, const std::string &) const;
    bool operator()(const std::string &, const std::string &) const;
};

#endif

