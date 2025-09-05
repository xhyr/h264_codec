#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

struct BlockUtil
{
	static uint32_t CalculateBlockIndex(uint32_t block_8x8, uint32_t block_4x4);
};

__codec_end