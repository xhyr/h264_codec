#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

struct CommonConstantValues
{
	constexpr static uint32_t s_zero_bytes_short_start_code{ 2 };
	constexpr static uint32_t s_zero_bytes_start_code{ 3 };

	constexpr static uint32_t s_mb_width{ 16 };
	constexpr static uint32_t s_mb_height{ 16 };
	constexpr static uint32_t s_block_width{ 4 };
	constexpr static uint32_t s_block_height{ 4 };

	constexpr static uint8_t s_min_color_value{ 0 };
	constexpr static uint8_t s_max_color_value{ 255 };
	constexpr static uint8_t s_default_color_value{ 128 };

	constexpr static uint8_t s_min_qp{ 0 };
	constexpr static uint8_t s_max_qp{ 51 };
};

__codec_end