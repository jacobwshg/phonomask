#ifndef PHMASK_RULE_H
#define PHMASK_RULE_H

#include <vector>
#include <string>
#include <string_view>

/* 

 IMPORTANT
 A RuleParts object must be outlived by the relevant rule string

 */

namespace Phmask
{
    struct RuleParts
    {
        std::string_view A;
        std::string_view B;
        std::vector<std::string_view> X;
        std::vector<std::string_view> Y;

        explicit RuleParts(const std::string &);
        std::string str(void);
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

