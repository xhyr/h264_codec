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
	SMB8x8 = 4,
	SMB8x4 = 5,
	SMB48x8 = 6,
	SMB4x4 = 7,
	P8x8 = 9,
	I4 = 9, 
	I16 = 10
};

__codec_end