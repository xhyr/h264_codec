#include "data_util.h"

__codec_begin

std::vector<uint8_t> DataUtil::ObtainDataInBlock(std::shared_ptr<uint8_t[]> source_data, uint32_t x, uint32_t y, uint32_t block_width, uint32_t block_height, uint32_t width)
{
	std::vector<uint8_t> target_data;
	target_data.reserve(block_width * block_height);
	auto source_data_ptr = source_data.get();
	for (uint32_t src_y = y; src_y < y + block_height; ++src_y)
		for (uint32_t src_x = x; src_x < x + block_width; ++src_x)
			target_data.push_back(source_data_ptr[src_y * width + src_x]);
	return target_data;
}

void DataUtil::SetData(std::shared_ptr<uint8_t[]> total_data, std::shared_ptr<uint8_t[]> block_data, uint32_t x, uint32_t y, uint32_t block_width, uint32_t block_height, uint32_t width)
{
	for (uint32_t src_y = y; src_y < y + block_height; ++src_y)
		for (uint32_t src_x = x; src_x < x + block_width; ++src_x)
			total_data.get()[src_y * width + src_x] = block_data.get()[(src_y - y) * block_width + (src_x - x)];
}

__codec_end

