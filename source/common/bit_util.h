#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

struct BitUtil
{
	static void SetValue(uint32_t& value, uint32_t bit_pos, uint8_t bit_value);

};

__codec_end