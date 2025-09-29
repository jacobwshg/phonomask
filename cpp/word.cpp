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
    try_rule_from_pos(const WordRep &, const Rule &, 
                      std::size_t, std::size_t, std::size_t, std::size_t);
}

std::vector<std::string> 
Phmask::
word_to_segments(const std::string &word)
{
    using UNISTR = icu::UnicodeString;

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
        case U'#':
            // Data shouldn't need explicit word boundary symbols;
            // ignore
            break;
        case U'ˈ':
        case U'ˌ':
            // stressed syllable boundary
            [[fallthrough]];
        case U'.':
            // (unstressed) syllable boundary
            u_segbuf = c;
            segments.emplace_back(unistr_to_str(u_segbuf));
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

    segments.emplace_back("#");
    
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
 Resume evaluating a rule's conditions from intermediate
 positions within WORD_REP and RULE's X and Y parts.
 This recursive design simplifies skipping reserved symbols 
 and possibly optional segments.
 */
bool
Phmask::
try_rule_from_pos(const Phmask::WordRep &word_rep, const Phmask::Rule &rule,
                  std::size_t wxpos, std::size_t wypos,
                  std::size_t rxpos, std::size_t rypos)
{
    std::size_t wlen {word_rep.seg_reps.size()}, 
                rxlen {rule.X.size()}, 
                rylen {rule.Y.size()};

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

        if (rx.masks.test(wx.feat_mtx) || (rx_issb && wx_issb))
        // Exact segment match, or syllable boundary match
        {
            wxpos = update_pos(wxpos, wlen, true);
            rxpos = update_pos(rxpos, rxlen, true);
        }
        else if ((!rx_issb) && wx_issb)
        // Rule element doesn't specify syllable boundary
        // but "segment" is syllable boundary (should skip)
        {
            wxpos = update_pos(wxpos, wlen);
        }
        else
        {
            return false;
        }
        return try_rule_from_pos(word_rep, rule, wxpos, wypos, rxpos, rypos);
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

        if (ry.masks.test(wy.feat_mtx) || (ry_issb && wy_issb))
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
        return try_rule_from_pos(word_rep, rule, wxpos, wypos, rxpos, rypos);
    }

    // Rule elements exhausted, all matching segments
    return true;
}


void
Phmask::
WordRep::housekeep(void)
{
    if (!this->isdirty)
    {
        return;
    }
    constexpr feat_mtx_t empty_fm {0u};
    std::size_t cur_size {seg_reps.size()};
    std::vector<SegRep> seg_reps_tmp {};
    seg_reps_tmp.reserve(cur_size);

    for (std::size_t i {0}; i < cur_size; ++i)
    {
        feat_mtx_t insert_fm {this->seg_reps[i].insert_before};
        if (insert_fm.any() && !insert_fm.test(this->null_bit))
        // Exists segment to be inserted before position I
        {
            seg_reps_tmp.emplace_back(
                SegRep
                {
                    insert_fm,
                    empty_fm
                }
            );
            this->seg_reps[i].insert_before = empty_fm;
        }
        if (!this->seg_reps[i].feat_mtx.test(this->null_bit))
        // Segment at position I not deleted
        {
            seg_reps_tmp.emplace_back(std::move(this->seg_reps[i]));
        }
    }

    seg_reps_tmp.shrink_to_fit();
    this->seg_reps = std::move(seg_reps_tmp);
}
/*
Phmask::WordRep &
Phmask::
WordRep::apply_rule(const Phmask::Rule &rule)
{
    std::size_t wlen {this->seg_reps.size()},
                rxlen {rule.X.size()},
                rylen {rule.Y.size()};
    for (std::size_t apos {rxlen}; apos < wlen - rylen; ++apos)
    {
        std::size_t wxpos {apos > 0 ? apos - 1 : wlen},
                    wypos {apos < wlen ? apos : wlen},
                    rxpos {rxlen > 0 ? rxlen - 1 : rxlen};
                    rypos {rylen > 0 ? rylen - 1 : rylen};
        if rule.A.isnull(rule.null_bit)
        // Insertion rule
        {
            
        }
    }

    return *this;
}
*/


