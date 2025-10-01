#include "word.h"
#include "utils.h"
#include "feat_mtx.h"
#include <unicode/unistr.h>
#include <unicode/uchar.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>


namespace Phmask
{
    std::size_t
    update_pos(std::size_t, std::size_t, bool decr);

    bool
    try_rule_context(const WordRep &, const Rule &, 
                     std::size_t, std::size_t, std::size_t, std::size_t);
}

std::vector<std::string> 
Phmask::
word_to_segments(const std::string &word)
{
    using UNISTR = icu::UnicodeString;

    // Pad word begin with word boundary symbol
    std::vector<std::string> segments { "#", };
    UNISTR u_segbuf {};
    bool tied {false};

    UNISTR u_word {UNISTR::fromUTF8(word)};
    std::int32_t uwlen {u_word.length()};
    for (std::int32_t i {0}; i < uwlen; ++i)
    {
        UChar32 c {u_word.char32At(i)};
        UCharCategory ccateg {U_CHAR_CATEGORY_COUNT};
        switch (c)
        {
        case U'(':
        case U')':
            // Don't support optional segments in data
            break;
        case U'#':
            // Word boundary (maybe one data entry can span across words?)
        case U'ˈ':
        case U'ˌ':
            // Stressed syllable boundary
        case U'.':
            // (Unstressed) syllable boundary

            // Flush previous segment
            /*
               If the following pair of statements were switched,
               a symbol will incorrectly overwrite the previous segment 
               and be flushed twice
             */
            segments.emplace_back(unistr_to_str(u_segbuf));
            u_segbuf = c;
            break;
        case U'͡':
        case U'͜':
            tied = true;
            u_segbuf += c;
            break;
        default:
            ccateg = static_cast<UCharCategory>(u_charType(c));
            switch (ccateg)
            {
            /*
               If a full character is encountered, in order for it to be 
               recorded into a segment, it must be at the start of the segment 
               or else be tied to the previous full character 
               (plus possible marks and modifiers, which are simply 
               appended to that character).
               In all other cases, the full character belongs to the 
               next segment; flush the currently recorded characters.
               `U_LOWERCASE_LETTER` captures non-clicks and the bilabial 
               clicks, and `U_OTHER_LETTER` captures the remaining clicks.
             */
            case U_LOWERCASE_LETTER:
            case U_OTHER_LETTER:
                if (u_segbuf.isEmpty() || tied)
                {
                    u_segbuf += c;
                    tied = false;
                }
                else
                {
                    segments.emplace_back(unistr_to_str(u_segbuf));
                    u_segbuf = c; 
                }
                break;
            case U_NON_SPACING_MARK:
            case U_MODIFIER_LETTER:
                u_segbuf += c; 
                break;
            default:
                break;
            }
            break;
        }
    }
    if (!u_segbuf.isEmpty())
    {
        segments.emplace_back(unistr_to_str(u_segbuf));
    }
    // Pad word end with word boundary symbol
    segments.emplace_back("#");

    /*
    std::cout <<"word_to_segments(): ";
    for (std::size_t i{0};i<segments.size();++i){std::cout<<segments[i]<<" ";}
    std::cout<<"\n";
    */

    return segments;
}

/*
 Increment/decrement POS used to index a vector,
 safely overflowing to ENDPOS (vector.size()) if out of range
 DECR: true if decrementing
 */
std::size_t
Phmask::
update_pos(std::size_t pos, std::size_t endpos, bool decr = false)
{
    if (pos == endpos)
    {
        return pos;
    }
    if (decr)
    {
        return pos == 0 ? endpos : --pos;
    }
    else
    {
        return pos == endpos ? pos : ++pos;
    }
}

/*
 Resume scanning a word against a rule's conditions 
 from intermediate positions within WORD_REP and RULE's 
 X and Y parts.
 This recursive design simplifies skipping reserved symbols 
 and possibly optional segments.
 */
bool
Phmask::
try_rule_context(const Phmask::WordRep &word_rep, const Phmask::Rule &rule,
                 std::size_t wxpos, std::size_t wypos,
                 std::size_t rxpos, std::size_t rypos)
{
    std::size_t wlen {word_rep.seg_reps.size()}, 
                rxlen {rule.X.size()}, 
                rylen {rule.Y.size()};

    if ((rxpos >= rxlen) && (rypos >= rylen))
    // Rule elements exhausted, all matching segments
    {
        return true;
    }

    if (rxpos < rxlen)
    // Element available in RULE's X
    {
        if (wxpos >= wlen)
        // No remaining segment on the left in WORD_REP
        {
            return false;
        }

        const SegRep &wx {word_rep.seg_reps[wxpos]};
        const RuleElem &rx {rule.X[rxpos]};

        bool rx_issb {rx.issb(rule.sb_bit)};
        bool wx_issb {wx.issb(word_rep.sb_bit)};

        if (rx.masks.test_fm(wx.feat_mtx) || (rx_issb && wx_issb))
        // Exact segment match, or syllable boundary match
        {
            wxpos = update_pos(wxpos, wlen, true);
            rxpos = update_pos(rxpos, rxlen, true);
        }
        else if ((!rx_issb) && wx_issb)
        // Rule element doesn't specify syllable boundary
        // but "segment" is syllable boundary (should skip)
        {
            wxpos = update_pos(wxpos, wlen, true);
        }
        else
        {
            return false;
        }
    }

    if (rypos < rylen)
    // Element available in RULE's Y
    {
        if (wypos >= wlen)
        // No remaining segment on the right in WORD_REP
        {
            return false;
        }
        const SegRep &wy {word_rep.seg_reps[wypos]};
        const RuleElem &ry {rule.Y[rypos]};

        bool ry_issb {ry.issb(rule.sb_bit)};
        bool wy_issb {wy.issb(word_rep.sb_bit)};

        if (ry.masks.test_fm(wy.feat_mtx) || (ry_issb && wy_issb))
        // Exact segment match, or syllable boundary match
        {
            wypos = update_pos(wypos, wlen);
            rypos = update_pos(rypos, rylen);
        }
        else if ((!ry_issb) && wy_issb)
        // Rule element doesn't specify syllable boundary
        // but "segment" is syllable boundary (should skip)
        {
            wypos = update_pos(wypos, wlen);
        }
        else
        {
            return false;
        }
    }

    return try_rule_context(word_rep, rule, wxpos, wypos, rxpos, rypos);
}

void
Phmask::
WordRep::housekeep(void)
{
    if (!this->isdirty)
    {
        return;
    }
    std::size_t cur_size {this->seg_reps.size()};
    std::vector<SegRep> seg_reps_tmp {};
    seg_reps_tmp.reserve(cur_size);

    for (std::size_t i {0}; i < cur_size; ++i)
    {
        feat_mtx_t insert_fm {this->seg_reps[i].insert_before_fm};
        if (insert_fm.any() && !insert_fm.test(this->null_bit))
        // Exists segment to be inserted before position I
        {
            seg_reps_tmp.emplace_back(
                SegRep
                {
                    insert_fm,
                    EMPTY_FEAT_MTX,
                }
            );
            this->seg_reps[i].insert_before_fm = EMPTY_FEAT_MTX;
        }
        if (!this->seg_reps[i].feat_mtx.test(this->null_bit))
        // Segment at position I not deleted
        {
            seg_reps_tmp.emplace_back(std::move(this->seg_reps[i]));
        }
    }

    seg_reps_tmp.shrink_to_fit();
    this->seg_reps = std::move(seg_reps_tmp);
    this->apply_at = std::vector<bool>(seg_reps.size(), false);
    this->isdirty = false;
}

Phmask::WordRep &
Phmask::
WordRep::apply_rule(const Phmask::Rule &rule)
{
    std::size_t wlen {this->seg_reps.size()},
                rxlen {rule.X.size()},
                rylen {rule.Y.size()};

    // Whether the rule can apply at any position at all
    bool can_apply {false};

    // true if the rule is insertion or deletion, 
    // respectively indicated by the A or B element
    // being the null segment symbol
    bool isinsert {rule.A.isnull(rule.null_bit)},
         isdelete {rule.B.isnull(rule.null_bit)};

    for (std::size_t pos {rxlen}; pos < wlen - rylen; ++pos)
    {
        const SegRep &cur_seg {this->seg_reps[pos]};
        const feat_mtx_t cur_fm {cur_seg.feat_mtx};

        if ( (!rule.A.masks.test_fm(cur_fm)) && !isinsert )
        // Current segment does not match A in rule,
        // and rule is not insertion;
        // Rule does not affect current segment
        {
            continue;
        }

        // Starting positions to begin recursively testing the rule
        // outwards from the current segment (A)'s position
        std::size_t 
            // X in word: one position left of A
            wxpos {pos > 0 ? pos - 1 : wlen},
            // Y in word: same as A (unchanged if rule is insertion)
            wypos {pos},
            // X in rule: rightmost position
            rxpos {rxlen > 0 ? rxlen - 1 : rxlen},
            // Y in rule: leftmost position
            rypos {0};
        if (!isinsert)
        // Not an insertion rule
        {
            ++wypos;
        }
        
        if (!try_rule_context(*this, rule, wxpos, wypos, rxpos, rypos))
        // Adjacent segments do not match rule elements
        {
            continue;
        }

        // Finally, all segments match the rule; mark current position
        this->apply_at[pos] = true;
        if (!can_apply)
        {
            can_apply = true;
        }
    }

    for (std::size_t pos {0}; pos < wlen; ++pos)
    {
        SegRep &cur_seg {this->seg_reps[pos]};
        if (this->apply_at[pos])
        {
            if (isinsert)
            {
                rule.B.masks.set_fm(cur_seg.insert_before_fm);
            }
            else
            {
                rule.B.masks.set_fm(cur_seg.feat_mtx);
            }
        }
    }

    if (can_apply && (isinsert || isdelete))
    {
        this->isdirty = true;
    }
    this->housekeep();

    return *this;
}

