#include "utils.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string_view>
#include <stdexcept>

std::unique_ptr<std::istream>
Phmask::
table_stream_ptr(const std::string &path)
{
    return std::make_unique<std::ifstream>(path);
}

std::vector<std::string>
Phmask::
fields_from_row(std::istream &is)
{
    std::vector<std::string> fields {};
    std::string field {};
    char c {};

    while (is.get(c))
    {
        switch (c)
        {
        case '\r':
            break;
        case '\n':
        case ',':
            fields.emplace_back(field);
            field.clear();
            if (c == '\n')
            {
                goto done;
            }
            break;
        default:
            field += c;
            break;
        }
    }

    done:
    if (!field.empty())
    {
        // EOF without trailing newline
        fields.emplace_back(field);
    }
    return fields;
}

std::string
Phmask::
unistr_to_str(const icu::UnicodeString &unistr)
{
    std::string s {};
    s.reserve(unistr.length());
    unistr.toUTF8String(s);
    return s;
}

