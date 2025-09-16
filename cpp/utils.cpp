#include "utils.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

std::unique_ptr<std::istream>
Phmask::
table_stream_ptr(std::string &path)
{
    return std::make_unique<std::ifstream>(path);
}

void
Phmask::
fields_from_row(std::istream &is, std::vector<std::string> &fields)
{
    fields.clear();
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
                return;
            }
            break;
        default:
            field += c;
            break;
        }
    }
    // EOF
    if (!field.empty())
    {
        fields.emplace_back(field);
        field.clear();
    }
}

std::vector<std::string>
Phmask::
fields_from_row(std::istream &is)
{
    std::vector<std::string> fields {};
    Phmask::fields_from_row(is, fields);
    return fields;
}

