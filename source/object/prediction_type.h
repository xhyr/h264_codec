#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

enum class Intra16PredictionType : uint8_t
{
	Vertical = 0,
	Horizontal = 1,
	DC = 2,
	Plane = 3
};

__codec_end