#include "rule.h"
#include "svutils.h"
#include <vector>
#include <string>
#include <string_view>
#include <cstddef>
#include <unordered_set>
#include <sstream>

std::vector<std::string_view>
Phmask::
rule_str_toks(const std::string &rule_str)
{
    std::vector<std::string_view> rule_toks {};
    std::size_t r_len {rule_str.size()};

    std::string_view tok {""};
    std::size_t tok_begin {0}, // Initial pos in rule token
                tok_end {0};   // Pos after rule token

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
                    rule_toks.emplace_back(tok);
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
        rule_toks.emplace_back(tok);
    }

    return rule_toks;
}

std::vector<std::string_view>
Phmask::
parse_feature_bundle_str(const std::string_view fb_str)
{
    std::vector<std::string_view> toks {};
    std::size_t fb_len {fb_str.size()};
    std::size_t tok_begin {0},
                tok_end {0};
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

Phmask::
Rule::Rule(const Phmask::FeatureProfile &profile, 
           const std::string &rule_str) :
    A {}, B {}, X {}, Y {}
{
    const static std::unordered_set<std::string, SvStrHash, SvStrEq> 
        arrows
    {
        "→", "->", ">",
    };

    std::vector<std::string_view> rule_toks
    {
        Phmask::rule_str_toks(rule_str)
    };

    enum class State
    {
        A, B, X, Y,
    }
    parser_state {State::A};

    for (std::string_view &tok : rule_toks)
    {
        switch (parser_state)
        {
        case State::A:
            if (arrows.find(tok) != arrows.end())
            {
                parser_state = State::B;
            }
            else
            {
                A = profile.rule_tok_to_masks(tok);
            }
            break;
        case State::B:
            if (tok == "/")
            {
                parser_state = State::X;
            }
            else
            {
                B = profile.rule_tok_to_masks(tok);
            }
            break;
        case State::X:
            if (tok == "_")
            {
                parser_state = State::Y;
            }
            else
            { 
                X.emplace_back(profile.rule_tok_to_masks(tok));
            }
            break;
        case State::Y:
            Y.emplace_back(profile.rule_tok_to_masks(tok));
            break;
        default:
            break;
        }
    }
}

std::string
Phmask::
Rule::masks_str(void) const
{
    std::ostringstream rule_sstrm {};
    rule_sstrm 
        << "Rule\n" 
        << "A:\n" << A.str()
        << "B:\n" << B.str()
        << "X:\n";
    for (const FeatureBundleMasks &ms : X)
    {
        rule_sstrm << ms.str();
    }
    rule_sstrm << "Y:\n";
    for (const FeatureBundleMasks &ms : Y)
    {
        rule_sstrm << ms.str();
    }
    return rule_sstrm.str();
}

