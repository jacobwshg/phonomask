#include "apply.h"
#include "masks.h"
#include "feat_mtx.h"
#include <vector>
#include <cstddef>

void 
Phmask::
apply(const Phmask::Rule &rule, std::vector<Phmask::feat_mtx_t> &word_fms)
{
    std::size_t wordlen {word_fms.size()},
                xlen {rule.X.size()},
                ylen {rule.Y.size()};
    for (std::size_t i {xlen}; i < wordlen - ylen; ++i)
    {
        Phmask::feat_mtx_t cur_seg_fm {word_fms[i]};
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
}

