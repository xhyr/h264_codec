#pragma once

#include <cstdint>

#include "block_data.h"

__codec_begin

struct TransformUtil
{
	static BlockData<4, 4, int32_t> Hadamard(const BlockData<4, 4, int32_t>& block_data, int divide_factor = 2);

	static BlockData<2, 2, int32_t> Hadamard(const BlockData<2, 2, int32_t>& block_data);

	static BlockData<4, 4, int32_t> InverseHadamard(const BlockData<4, 4, int32_t>& block_data);

	static BlockData<2, 2, int32_t> InverseHadamard(const BlockData<2, 2, int32_t>& block_data);

	static BlockData<4, 4, int32_t> DCT(const BlockData<4, 4, int32_t>& block_data);

	static BlockData<4, 4, int32_t> InverseDCT(const BlockData<4, 4, int32_t>& block_data);

};

__codec_end