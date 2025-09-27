#ifndef PHMASK_RULE_H
#define PHMASK_RULE_H

//#include "feature_profile.h"
#include "masks.h"
#include <vector>
#include <string>
#include <string_view>

namespace Phmask
{

    struct Rule
    {
        FeatureBundleMasks A;
        FeatureBundleMasks B;
        std::vector<FeatureBundleMasks> X;
        std::vector<FeatureBundleMasks> Y;

        //Rule(const FeatureProfile &profile, const std::string &rule_str);

        std::string masks_str(void) const;
    };

    std::vector<std::string_view>
    rule_str_toks(const std::string &rule_str);

    std::vector<std::string_view>
    parse_feature_bundle_str(const std::string_view);
}

#endif

