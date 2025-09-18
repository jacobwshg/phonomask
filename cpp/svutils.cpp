#include "svutils.h"
#include <cstddef>
#include <string>
#include <string_view>

std::size_t 
SvStrHash::operator()(const std::string_view sv) const
{
    return std::hash<std::string_view>{}(sv);
}

std::size_t 
SvStrHash::operator()(const std::string &str) const
{
    return std::hash<std::string>{}(str);
}

bool 
SvStrEq::operator()(const std::string_view sv, const std::string &str) const
{
    return sv == str;
}

bool 
SvStrEq::operator()(const std::string &str1, const std::string &str2) const
{
    return str1 == str2;
}


