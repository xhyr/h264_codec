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
	YUVFrame(uint32_t _width, uint32_t _height, const std::string& file_path, uint32_t tick = 0);
	YUVFrame(uint32_t _width, uint32_t _height);

	bool Serial(const std::string& file_path);

	bool Unserial(uint32_t width, uint32_t height, const std::string& file_path, uint32_t tick);

	uint8_t GetLumaValue(int x, int y) const;

	uint32_t width, height;
	std::shared_ptr<uint8_t[]> y_data, u_data, v_data;
};

__codec_end