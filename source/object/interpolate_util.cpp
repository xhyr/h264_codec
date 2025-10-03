#include "interpolate_util.h"

#include "math_util.h"

__codec_begin

std::vector<uint8_t> InterpolateUtil::InterpolateChromaBlock(std::shared_ptr<uint8_t[]> source_data, int start_x, int start_y, int block_width, int block_height, int width, int height)
{
	std::vector<uint8_t> out_data;
	out_data.reserve(block_width * block_height);
	for (uint32_t j = 0; j < block_height; ++j)
	{
		uint32_t y = MathUtil::Clamp<uint32_t>(start_y + j * 8, 0, (height - 1) * 8);

		for (uint32_t i = 0; i < block_width; ++i)
		{
			uint32_t x = MathUtil::Clamp<uint32_t>(start_x + i * 8, 0, (width - 1) * 8);
			auto value = GetInterploatedChromaPixel(source_data, x, y, width, height);
			out_data.push_back(value);
		}
	}
	return out_data;
}

uint8_t InterpolateUtil::GetInterploatedChromaPixel(std::shared_ptr<uint8_t[]> source_data, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	uint8_t value = 0;
	uint32_t dx = x % 8;
	uint32_t dy = y % 8;
	uint32_t cx = x / 8;
	uint32_t cy = y / 8;
	if (dx == 0 && dy == 0)
	{
		value = source_data.get()[cy * width + cx];
	}
	else if (x % 8 == 0)
	{
		auto up_value = source_data.get()[cy * width + cx];
		auto down_value = source_data.get()[(cy + 1) * width + cx];
		value = MathUtil::RightShift(up_value * (8 - dy) + down_value * dy, 3);
	}
	else if (y % 8 == 0)
	{
		auto left_value = source_data.get()[cy * width + cx];
		auto right_value = source_data.get()[cy * width + cx + 1];
		value = MathUtil::RightShift(left_value * (8 - dx) + right_value * dx, 3);
	}
	else
	{
		auto a_value = source_data.get()[cy * width + cx];
		auto b_value = source_data.get()[cy * width + cx + 1];
		auto c_value = source_data.get()[(cy + 1) * width + cx];
		auto d_value = source_data.get()[(cy + 1) * width + cx + 1];
		value = MathUtil::RightShift((8 - dx) * (8 - dy) * a_value + dx * (8 - dy) * b_value + (8 - dx) * dy * c_value + dx * dy * d_value, 6);
	}

	return value;
}

__codec_end