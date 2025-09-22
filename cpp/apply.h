#ifndef PHMASK_APPLY_H
#define PHMASK_APPLY_H

#include "rule.h"
#include "feat_mtx.h"
#include <vector>

namespace Phmask
{
    void apply(const Rule &, std::vector<feat_mtx_t> &);
}

#endif

