#pragma once

#include <cstdint>
#include <memory>

#include "global_defines.h"

__codec_begin

//yuv420p
struct YUVFrame
{
	uint32_t width, height;
	std::shared_ptr<uint8_t[]> y_data, u_data, v_data;
};

__codec_end