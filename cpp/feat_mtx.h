#ifndef FEAT_MTX_H
#define FEAT_MTX_H

#include <cstddef>
#include <bitset>

namespace Phmask
{
	// Max number of valid features to support
	static constexpr std::size_t MAX_NUM_FEATS { 40 };

	// Add some length for reserved symbol markers
	using feat_mtx_t = std::bitset< MAX_NUM_FEATS + 4 >;

	static constexpr feat_mtx_t EMPTY_FEAT_MTX { 0UL };
	static constexpr feat_mtx_t FLIPPED_EMPTY_FEAT_MTX { ~0UL };

	// Invalid placeholder feature matrix that can be returned
	// when the segment-feature matrix map is keyed with an unknown segment.
	// Using the empty mtx is OK so long as the null segment
	// is specified with the null bit.
	// However, the flipped empty mtx is more significantly different
	// from any valid mtx, thus its use for this purpose.
	static constexpr feat_mtx_t INVALID_FEAT_MTX { FLIPPED_EMPTY_FEAT_MTX };
}

#endif

