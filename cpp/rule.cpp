#include "rule.h"
#include "svutils.h"
#include <vector>
#include <string>
#include <string_view>
#include <cstddef>
#include <unordered_set>
#include <sstream>

////////
#include <iostream>

std::vector<std::string_view>
Phmask::
rule_str_to_large_toks(const std::string &rule_str)
{
    std::vector<std::string_view> large_toks {};
    std::size_t r_len {rule_str.size()};

    std::string_view tok {""};
    std::size_t tok_begin {0}, // Initial pos in large token
                tok_end {0};   // Pos after large token

    bool in_feat_bdl {false};  // Whether current pos in feature bundle

    for (std::size_t i {0}; i <= r_len; ++i)
    {
        char c {rule_str[i]};
        switch (c)
        {
        case ' ':
            if (!in_feat_bdl)
            {
                if (tok_end > tok_begin)
                { 
                    tok = rule_str;
                    tok.remove_prefix(tok_begin); 
                    tok.remove_suffix(r_len - tok_end);
                    large_toks.emplace_back(tok);
                }
                tok_end = (tok_begin = i + 1);
                continue;
            }
            [[fallthrough]];
        default:
            if (c == '[')
            {
                in_feat_bdl = true;
            }
            else if (c == ']')
            {
                in_feat_bdl = false;
            }
            tok_end = i + 1;
            break;
        }
    }
    if (tok_end > tok_begin)
    {
        tok = rule_str; 
        tok.remove_prefix(tok_begin); 
        tok.remove_suffix(r_len - tok_end);
        large_toks.emplace_back(tok);
    }

    return large_toks;
}

Phmask::
RuleParts::RuleParts(const std::string &rule_str) :
    A {""}, B {""}, X {}, Y {}
{
    const static std::unordered_set<std::string, SvStrHash, SvStrEq> 
        arrows
    {
        "→", "->", ">",
    };

    std::vector<std::string_view> large_toks
    {
        Phmask::rule_str_to_large_toks(rule_str)
    };

    enum class State
    {
        A, B, X, Y,
    }
    parser_state {State::A};

    for (std::string_view &large_tok : large_toks)
    {
        switch (parser_state)
        {
        case State::A:
            if (arrows.find(large_tok) != arrows.end())
            {
                parser_state = State::B;
            }
            else
            {
                A = large_tok;
            }
            break;
        case State::B:
            if (large_tok == "/")
            {
                parser_state = State::X;
            }
            else
            {
                B = large_tok;
            }
            break;
        case State::X:
            if (large_tok == "_")
            {
                parser_state = State::Y;
            }
            else
            { 
                X.emplace_back(large_tok);
            }
            break;
        case State::Y:
            Y.emplace_back(large_tok);
            break;
        default:
            break;
        }
    }
}

std::string 
Phmask::
RuleParts::str(void)
{
    std::ostringstream rp_sstrm {};
    rp_sstrm 
        << "A: " << A << "\n"
        << "B: " << B << "\n"
        << "X: ";
    for (const std::string_view &x_elem : X)
    {
        rp_sstrm << "`" << x_elem << "`";
    }
    rp_sstrm << "\nY: ";
    for (const std::string_view &y_elem : Y)
    {
        rp_sstrm << "`" << y_elem << "`";
    }
    rp_sstrm << "\n";
    return rp_sstrm.str();
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
        char c {fb_str[i]};
        switch (c)
        {
        case '[':
        case ']':
        case ',':
        case ' ':
            if (tok_end > tok_begin)
            {
                std::string_view tok {fb_str};
                tok.remove_prefix(tok_begin);
                tok.remove_suffix(fb_len - tok_end);
                toks.emplace_back(tok);
            }
            tok_end = (tok_begin = i + 1);
            break;
        default:
            tok_end = i + 1;
            break;
        }
    }
    return toks;
}

