#ifndef FEAT_MTX_H
#define FEAT_MTX_H

#include <cstddef>
#include <bitset>

namespace Phmask
{
    using feat_mtx_t = std::bitset<48>;

    constexpr static feat_mtx_t EMPTY_FEAT_MTX {0u};
    constexpr static feat_mtx_t FLIPPED_EMPTY_FEAT_MTX {~0u};
}

#endif

