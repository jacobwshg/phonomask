#ifndef PHMASK_RULE_H
#define PHMASK_RULE_H

#include "masks.h"
#include "feature_profile.h"
#include <vector>
#include <string>
#include <string_view>

/********************
 IMPORTANT
     A RuleParts object must be outlived by the relevant rule string
 ********************/

namespace Phmask
{
    class FeatureProfile;

    struct RuleParts
    {
        std::string_view A;
        std::string_view B;
        std::vector<std::string_view> X;
        std::vector<std::string_view> Y;

        explicit RuleParts(const std::string &);
        std::string str(void) const;
    };

    struct Rule
    {
        Phmask::FeatureBundleMasks A;
        Phmask::FeatureBundleMasks B;
        std::vector<Phmask::FeatureBundleMasks> X;
        std::vector<Phmask::FeatureBundleMasks> Y;

        Rule(const Phmask::FeatureProfile &profile, const std::string &rule_str);

        std::string masks_str(void) const;
    };

    std::vector<std::string_view>
    rule_str_toks(const std::string &rule_str);

    std::vector<std::string_view>
    parse_feature_bundle_str(const std::string_view);
}

#endif

