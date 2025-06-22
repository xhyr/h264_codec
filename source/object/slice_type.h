#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

enum class SliceType : uint8_t
{
	P = 0 + 5, 
	B = 1 + 5,
	I = 2 + 5,
	SP = 3 + 5,
	SI = 4 + 5
};

__codec_end