#ifndef FEAT_MTX_H
#define FEAT_MTX_H

#include <cstddef>
#include <bitset>

namespace Phmask
{
    using feat_mtx_t = std::bitset<48>;

    static constexpr feat_mtx_t EMPTY_FEAT_MTX { 0UL };
    static constexpr feat_mtx_t FLIPPED_EMPTY_FEAT_MTX { ~0UL };
}

#endif

