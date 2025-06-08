#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "global_defines.h"

__codec_begin

struct ColorData;

//yuv420p
struct YUVFrame
{
	YUVFrame();
	explicit YUVFrame(std::shared_ptr<ColorData> color_data);

	bool Serial(const std::string& file_path);

	uint32_t width, height;
	std::shared_ptr<uint8_t[]> y_data, u_data, v_data;
};

__codec_end