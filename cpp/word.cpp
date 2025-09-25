#include "word.h"
#include "utf8.h"
#include <unicode/unistr.h>
#include <unicode/uchar.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>

using UNISTR = icu::UnicodeString;

std::vector<std::string> 
Phmask::
word_to_segments(const std::string &word)
{
    std::vector<std::string> segments {};
    //std::vector<std::string> segments { "#", };
    std::string segbuf {}; 
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
            break;
        case U'ˈ':
        case U'ˌ':
            // TODO: stress
            [[fallthrough]];
        case U'.':
            // TODO: syllable
            //segments.emplace_back("$");
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
               recorded into a segment, it must be at the start of the segment               or else be tied to the previous full character 
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
                    segbuf.clear();
                    u_segbuf.toUTF8String(segbuf);
                    segments.emplace_back(segbuf);
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
        segbuf.clear();
        u_segbuf.toUTF8String(segbuf);
        segments.emplace_back(segbuf);
    }

    // segments.emplace_back("#");
    
    return segments;
}

