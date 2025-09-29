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
                    insert_fm.test(this->wb_bit),
                    insert_fm.test(this->sb_bit),
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
    std::size_t wordlen {this->seg_reps.size()},
                xlen {rule.X.size()},
                ylen {rule.Y.size()};
    for (std::size_t i {0}; i < wordlen; ++i)
    {
        SegRep seg {this[i]};

        // TODO

        std::size_t xbegin {i - xlen};
        std::size_t ybegin {i + 1};
        if (!rule.A.test(cur_seg_fm))
        {
            goto done;
        }
        for (std::size_t x_i {0}; x_i < xlen; ++x_i)
        {
            if (!rule.X[x_i].test(word_fms[xbegin + x_i]))
            {
                goto done;
            }
        }
        for (std::size_t y_i {0}; y_i < ylen; ++y_i)
        {
            if (!rule.Y[y_i].test(word_fms[ybegin + y_i]))
            {
                goto done;
            }
        }
        word_fms[i] = rule.B.set(cur_seg_fm);

        done:
            continue;
    }


    return *this;
}

*/
