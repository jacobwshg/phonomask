#include "feature_profile.h"
#include "feat_idx_maps.h"
#include "seg_fm_maps.h"
#include "feat_mtx.h"
#include "rule.h"
#include "masks.h"
#include "word.h"
#include "utils.h"
#include <vector>
#include <string>
#include <string_view>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <unordered_set>
#include <cstddef>

void
Phmask::
FeatureProfile::add_reserved(void)
{
    // Add reserved symbols
    std::size_t 
        // 1 if null segment symbol
        null_bit {num_feats},
        // 1 if word boundary symbol
        wb_bit {null_bit + 1},
        // 1 if syllable boundary symbol
        sb_bit {wb_bit + 1},
        // 1 if <$> or <.>, 0 if <ˈ> or <ˌ>
        sb_ascii_bit {sb_bit + 1},
        // "significant" - 1 if <.> or <ˈ>, 0 if <$> or <ˌ>
        sb_sig_bit {sb_ascii_bit + 1};
    this->null_bit = null_bit;
    this->wb_bit = wb_bit;
    this->sb_bit = sb_bit;
    this->seg_fm_maps
        // null segment
        .add("∅", feat_mtx_t{0u}.set(null_bit))
        // word boundary
        .add("#", feat_mtx_t{0u}.set(wb_bit))
        // syllable boundary (rule)
        .add("$", feat_mtx_t{0u}.set(sb_bit).set(sb_ascii_bit))
        // syllable boundary (data, unstressed)
        .add(".", feat_mtx_t{0u}.set(sb_bit).set(sb_ascii_bit).set(sb_sig_bit))
        // syllable boundary (primary stress)
        .add("ˈ", feat_mtx_t{0u}.set(sb_bit).set(sb_sig_bit))
        // syllable boundary (secondary stress)
        .add("ˌ", feat_mtx_t{0u}.set(sb_bit));
}

Phmask::
FeatureProfile::FeatureProfile(const std::string &path):
    num_feats {0}, feat_idx_maps {}, seg_fm_maps {}, 
    null_bit {}, wb_bit {}, sb_bit {}
{
    std::unique_ptr<std::istream> table_sp 
    { 
        Phmask::table_stream_ptr(path) 
    };
    std::istream &table_strm {*table_sp};

    std::vector<std::string> header_row_fields
    {
        Phmask::fields_from_row(table_strm)
    };

    std::size_t num_cols {header_row_fields.size()};
    if (num_cols > 1)
    {
        this->num_feats = num_cols - 1;
    }
    if (this->num_feats > 40)
    {
        throw std::runtime_error("Currently supporting up to 40 features\n");
    }

    this->feat_idx_maps.populate(header_row_fields);
    this->seg_fm_maps.populate(table_strm);

    this->add_reserved();
}

const std::string &
Phmask::
FeatureProfile::feature_at(const std::size_t index) const
{
    return this->feat_idx_maps.feature_at(index);
}

std::size_t 
Phmask::
FeatureProfile::index_of(const std::string_view feature) const
{
    return this->feat_idx_maps.index_of(feature);
}

Phmask::feat_mtx_t 
Phmask::
FeatureProfile::feat_mtx_of(const std::string_view segment) const
{
    return this->seg_fm_maps.feat_mtx_of(segment);
}

const std::string &
Phmask::
FeatureProfile::segment_of(const feat_mtx_t feat_mtx) const
{
    return this->seg_fm_maps.segment_of(feat_mtx);
}

std::string
Phmask::
FeatureProfile::seg_effective_feats_str(const std::string &segment, 
                                        Phmask::feat_mtx_t ef_mask) const
{
    std::string ef_feats_str {"["};

    feat_mtx_t feat_mtx {this->feat_mtx_of(segment)};
    for (std::size_t idx {0}; idx < this->num_feats; ++idx)
    {
        if (ef_mask.test(idx))
        // Feature at IDX is effective
        {
            ef_feats_str += (feat_mtx.test(idx) ? "+" : "-");
            const std::string &feature {this->feat_idx_maps.feature_at(idx)};
            ef_feats_str += feature;
            ef_feats_str += ", ";
        }
    }
    ef_feats_str += "]";
    return ef_feats_str;
}

std::string
Phmask::
FeatureProfile::seg_feat_mtx_str(const std::string &segment) const
{
    return this->seg_effective_feats_str(segment, all_feats_mask());
}

std::string
Phmask::
FeatureProfile::seg_positive_feats_str(const std::string &segment) const
{
    return this->seg_effective_feats_str(segment,
                                         this->feat_mtx_of(segment));
}

Phmask::RuleElem
Phmask::
FeatureProfile::segment_to_rule_elem(std::string_view segment) const
{
    feat_mtx_t seg_feat_mtx {this->feat_mtx_of(segment)};

    return 
        RuleElem
        {
            FeatureBundleMasks 
            {
                Phmask::FLIPPED_EMPTY_FEAT_MTX,
                seg_feat_mtx
            }
        };
}

Phmask::RuleElem
Phmask::
FeatureProfile::feat_bundle_to_rule_elem(const std::string_view fb_str) const
{
    FeatureBundleMasks masks {};
    std::vector<std::string_view> fb_toks
    {
        Phmask::parse_feature_bundle_str(fb_str)
    };
    for (const std::string_view &tok : fb_toks)
    {
        std::size_t tok_len {tok.size()};
        std::string_view value {tok},  
                         feature {tok};
        value.remove_suffix(tok_len - 1);
        feature.remove_prefix(1);

        std::size_t feature_index {this->feat_idx_maps.index_of(feature)};

        // TODO: to support alpha, modify this part
        switch(value[0])
        {
        case '+':
            masks.add_positive(feature_index);
            break;
        case '-':
            masks.add_negative(feature_index);
            break;
        default:
            throw std::runtime_error("Feature bundle format not yet supported\n");
            break;
        }
    }
    return
        RuleElem { masks };
}

Phmask::RuleElem
Phmask::
FeatureProfile::rule_tok_to_elem(const std::string_view tok) const
{
    if (tok.find('[') != std::string::npos)
    {
        // Assume the token is a feature bundle
        return this->feat_bundle_to_rule_elem(tok);
    }
    else
    {
        return this->segment_to_rule_elem(tok);
    }
}

Phmask::Rule
Phmask::
FeatureProfile::rule_from_str(const std::string &rule_str) const
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

    Rule rule {};
    rule.null_bit = this->null_bit;
    rule.wb_bit = this->wb_bit;
    rule.sb_bit = this->sb_bit;

    for (std::string_view &tok : rule_toks)
    {
        if (tok.size() < 1)
        {
            continue;
        }
        switch (parser_state)
        {
        case State::A:
            if (arrows.find(tok) != arrows.end())
            {
                parser_state = State::B;
            }
            else
            {
                rule.A = this->rule_tok_to_elem(tok);
            }
            break;
        case State::B:
            if (tok == "/")
            {
                parser_state = State::X;
            }
            else
            {
                rule.B = this->rule_tok_to_elem(tok);
            }
            break;
        case State::X:
            if (tok == "_")
            {
                parser_state = State::Y;
            }
            else
            { 
                rule.X.emplace_back(this->rule_tok_to_elem(tok));
            }
            break;
        case State::Y:
            rule.Y.emplace_back(this->rule_tok_to_elem(tok));
            break;
        default:
            break;
        }
    }

    return rule;
}

Phmask::WordRep
Phmask::
FeatureProfile::word_rep_from_str(const std::string &word) const
{
    WordRep word_rep {};
    word_rep.null_bit = this->null_bit;
    word_rep.wb_bit = this->wb_bit;
    word_rep.sb_bit = this->sb_bit;

    std::vector<std::string> segments {Phmask::word_to_segments(word)};
    word_rep.seg_reps.reserve(segments.size());

    for (const std::string &segment : segments)
    {
        feat_mtx_t feat_mtx {this->feat_mtx_of(segment)};
        word_rep.seg_reps.emplace_back(
            SegRep
            {
                .feat_mtx {feat_mtx},
                .insert_before_fm {EMPTY_FEAT_MTX},
            }
        );
        word_rep.apply_at.emplace_back(false);
    }

    return word_rep;
}

std::string 
Phmask::
FeatureProfile::word_rep_to_str(const WordRep &word_rep) const
{
    std::ostringstream word_sstrm {};
    const std::vector<SegRep> &seg_reps {word_rep.seg_reps};
    for (std::size_t i {0}; i < seg_reps.size(); ++i)
    {
        feat_mtx_t cur_fm {seg_reps[i].feat_mtx};
        const std::string &segment {this->segment_of(cur_fm)};
        word_sstrm << segment;
    }
    return word_sstrm.str();
}

