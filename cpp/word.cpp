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
    std::string segbuf {}; 
    UNISTR u_segbuf {};
    bool tied {false};

    UNISTR u_word {UNISTR::fromUTF8(word)};
    int32_t uwlen {u_word.length()};
    for (std::int32_t i {0}; i < uwlen; ++i)
    {
        UChar32 c {u_word.char32At(i)};
        UCharCategory ccateg {U_CHAR_CATEGORY_COUNT};
        switch (c)
        {
        case U'(':
        case U')':
            break;
        // TODO: stress
        case U'ˈ':
        case U'ˌ': // " ˌ "
            break;
        // TODO: syllable
        case U'.':
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
            default:
                u_segbuf += c;
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
    
    return segments;
}


