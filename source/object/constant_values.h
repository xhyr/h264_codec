#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

struct ConstantValues
{
	const static uint32_t s_mb_width;
	const static uint32_t s_mb_height;
	const static uint32_t s_block_width;
	const static uint32_t s_block_height;

	const static uint8_t s_min_color_value;
	const static uint8_t s_max_color_value;
	const static uint8_t s_default_color_value;
};

__codec_end