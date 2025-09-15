#include "utils.h"
#include <iostream>
#include <vector>
#include <string>

void
Phmask::fields_from_row(std::istream &is, std::vector<std::string> &fields)
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
fields_from_row(std::istream &is)
{
    std::vector<std::string> fields {};
    Phmask::fields_from_row(is, fields);
    return fields;
}

