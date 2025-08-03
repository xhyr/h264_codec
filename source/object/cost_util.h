#pragma once

#include <cstdint>

#include "block_data.h"

__codec_begin

struct CostUtil
{
	static int CalculateSATD(const BlockData<16, 16, int32_t>& block_data);

	static int CalculateSATD(const BlockData<8, 8, int32_t>& block_data);

	static int CalculateSATD(const BlockData<4, 4, int32_t>& block_data);

};

__codec_end