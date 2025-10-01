#ifndef FEATURE_PROFILE_H
#define FEATURE_PROFILE_H

#include "feat_idx_maps.h"
#include "seg_fm_maps.h"
#include "feat_mtx.h"
#include "rule.h"
#include "masks.h"
#include "word.h"
#include <cstddef>
#include <string_view>

namespace Phmask
{
    struct Rule;
    struct FeatureBundleMasks;

    class FeatureProfile
    {
    public:
        std::size_t num_feats;
        FeatIdxMaps feat_idx_maps;
        SegFMMaps seg_fm_maps;

        explicit FeatureProfile(const std::string &);

        const std::string &feature_at(const std::size_t) const;
        std::size_t index_of(const std::string_view) const;
        feat_mtx_t feat_mtx_of(const std::string_view) const;
        const std::string &segment_of(const feat_mtx_t) const;

        std::string seg_feat_mtx_str(const std::string &) const;
        std::string seg_positive_feats_str(const std::string &) const;

        Rule rule_from_str(const std::string &) const;

        WordRep word_rep_from_str(const std::string &) const;
        std::string word_rep_to_str(const WordRep &) const;

    private:
        /* Indices beyond those used for features, used instead for
           distinguishing reserved symbols for null segments and 
           word/syllable boundary
         */
        std::size_t null_bit;
        std::size_t wb_bit;
        std::size_t sb_bit;

        // Add reserved symbols
        void add_reserved(void);

        feat_mtx_t all_feats_mask(void) const
        {
            feat_mtx_t all_feats_mask {0u};
            all_feats_mask.set();
            all_feats_mask = ~(all_feats_mask << num_feats);
            return all_feats_mask;
        }

        std::string 
        seg_effective_feats_str(const std::string &, Phmask::feat_mtx_t) const;

        RuleElem segment_to_rule_elem(std::string_view) const;
        RuleElem feat_bundle_to_rule_elem(const std::string_view) const;
        RuleElem rule_tok_to_elem(const std::string_view) const;
    };
}

#endif

