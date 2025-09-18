#include "utils.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

std::unique_ptr<std::istream>
Phmask::
table_stream_ptr(const std::string &path)
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

std::vector<std::string>
Phmask::
parse_feature_bundle_str(std::string &fb_str)
{
    std::vector<std::string> fb_toks {};
    std::string tokbuf {};

    for (const char &c : fb_str)
    {
        switch (c)
        {
        case '[':
        case ']':
            break;
        case ',':
        case ' ':
            if (!tokbuf.empty())
            {
                fb_toks.emplace_back(tokbuf);
                tokbuf.clear();
            }
            break;
        default:
            tokbuf += c;
            break;
        }
    }
    if (!tokbuf.empty())
    {
        fb_toks.emplace_back(tokbuf);
    }

    return fb_toks;
}


