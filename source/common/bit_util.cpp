#include "bit_util.h"

__codec_begin

void BitUtil::SetValue(uint32_t& value, uint32_t bit_pos, uint8_t bit_value)
{
	if (bit_value == 0)
		value &= 1 << bit_pos;
	else
		value |= 1 << bit_pos;
}

__codec_end

