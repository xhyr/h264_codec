#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

enum class PlaneType : uint8_t
{
	Luma = 0,
	Cb,
	Cr
};

__codec_end