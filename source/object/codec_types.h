#pragma once

#include "global_defines.h"

__codec_begin

enum class NaluType
{
	Slice = 1,
	IDR = 5,
	SPS = 7,
	PPS = 8
};

enum class NaluPriority
{
	DISPOSABLE = 0,
	LOW = 1,
	HIGH = 2,
	HIGHEST = 3
};

__codec_end