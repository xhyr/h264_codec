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

enum class Intra8ChromaPredictionType : uint8_t
{
	DC = 0,
	Horizontal,
	Vertical,
	Plane
};

__codec_end