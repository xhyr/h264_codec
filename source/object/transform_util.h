#pragma once

#include <cstdint>

#include "block_data.h"

__codec_begin

struct TransformUtil
{
	static BlockData<4, 4, int> Hadamard(const BlockData<4, 4, int>& block_data, int divide_factor = 2);

	static BlockData<4, 4, int> InverseHadamard(const BlockData<4, 4, int>& block_data);

	static BlockData<4, 4, int> DCT(const BlockData<4, 4, int>& block_data);

	static BlockData<4, 4, int> InverseDCT(const BlockData<4, 4, int>& block_data);

};

__codec_end