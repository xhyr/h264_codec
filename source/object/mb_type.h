#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

enum class MBType : uint8_t
{
	PSkip = 0,
	P16x16 = 1,
	P16x8 = 2,
	P8x16 = 3,
	I4 = 9, 
	I16 = 10
};

__codec_end