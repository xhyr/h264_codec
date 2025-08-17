#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

enum class IntraLumaType : uint8_t
{
	Intra16 = 0,
	Intra4
};

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
	VerticalRight,
	HorizontalDown,
	VerticalLeft,
	HorizontalUp,
	None
};

enum class IntraChromaPredictionType : uint8_t
{
	DC = 0,
	Horizontal,
	Vertical,
	Plane,
	None
};

__codec_end