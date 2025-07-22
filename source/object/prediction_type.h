#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

enum class Intra16LumaPredictionType : uint8_t
{
	Vertical = 0,
	Horizontal,
	DC,
	Plane
};

enum class Intra4LumaPredictionType : uint8_t
{
	Vertical = 0,
	Horizontal,
	DC,
	DownLeft,
	DownRight,
	VerticalLeft,
	HorizontalDown,
	VerticalRight,
	HorizontalUp
};

enum class IntraChromaPredictionType : uint8_t
{
	DC = 0,
	Horizontal,
	Vertical,
	Plane
};

__codec_end