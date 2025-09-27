#ifndef PHMASK_WORD_H
#define PHMASK_WORD_H

#include "feat_mtx.h"
#include <vector>
#include <string>

namespace Phmask
{
    std::vector<std::string> word_to_segments(const std::string &);

    std::vector<feat_mtx_t> 
    segments_to_feat_mtxs(const std::vector<std::string> &);

    std::string feat_mtxs_to_word(const std::vector<feat_mtx_t> &);
}

#endif

