#ifndef PHMASK_RULE_H
#define PHMASK_RULE_H

#include <vector>
#include <string>
#include <string_view>

namespace Phmask
{
    struct RuleParts
    {
        std::string_view A;
        std::string_view B;
        std::string_view X;
        std::string_view Y;

        RuleParts(const std::string &);
    };

    class Rule
    {
    public:
        Rule(void) = default;
        ~Rule(void) = default;
    };

    std::vector<std::string_view>
    rule_str_to_large_toks(const std::string &rule_str);

/*
    std::vector<std::string>
    parse_feature_bundle_str(std::string_view);
*/

    std::vector<std::string_view>
    parse_feature_bundle_str(std::string_view);
}

#endif

