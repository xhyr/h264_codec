#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

enum class Intra16PredictionType : uint8_t
{
	Vertical = 0,
	Horizontal,
	DC,
	Plane
};

__codec_end