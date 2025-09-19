#include "rule.h"
#include <vector>
#include <string>
#include <string_view>

std::vector<std::string_view>
Phmask::
rule_str_to_large_toks(const std::string &rule_str)
{
    std::vector<std::string_view> large_toks {};
    return large_toks; 
}

Phmask::
RuleParts::RuleParts(const std::string &rule_str)
{
    size_t part_begin {0};
    size_t part_end {0};

    enum 
    {
        A, B, X, Y,
    } 
    parser_state {A};

    parser_state = B;

}

/*
std::vector<std::string>
Phmask::
parse_feature_bundle_str(std::string_view fb_str)
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
*/

std::vector<std::string_view>
Phmask::
parse_feature_bundle_str(std::string_view fb_str)
{
    std::vector<std::string_view> toks {};
    std::size_t fb_len {fb_str.size()};
    std::size_t tok_begin {0}, tok_end {0};
    for (std::size_t i {0}; i < fb_len; ++i)
    {
        char c {};
        switch (c = fb_str[i])
        {
        case '[':
        case ']':
        case ',':
        case ' ':
            if (tok_end - tok_begin > 0)
            {
                std::string_view tok {fb_str};
                tok.remove_prefix(tok_begin);
                tok.remove_suffix(fb_len - tok_end);
                toks.emplace_back(tok);
            }
            tok_end = (tok_begin = i + 1);
            break;
        default:
            ++tok_end;
            break;
        }
    }
    return toks;
}

