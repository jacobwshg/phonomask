#ifndef SVUTILS_H
#define SVUTILS_H

#include <cstddef>
#include <string>
#include <string_view>

struct SvStrHash
{
    using is_transparent = void;

    std::size_t
    operator()(const std::string_view sv) const
    {
        return std::hash<std::string_view>{}(sv);
    }

    std::size_t
    operator()(const std::string &s) const
    {
        return std::hash<std::string>{}(s);
    }
};

struct SvStrEq
{
    using is_transparent = void;

    bool
    operator()(const std::string_view sv1, const std::string_view sv2) const
    {
        return sv1 == sv2;
    }

    bool
    operator()(const std::string_view sv, const std::string &s) const
    {
        return sv == s;
    }

    bool
    operator()(const std::string &s1, const std::string &s2) const
    {
        return s1 == s2;
    }
};

#endif

