#include "yuv_util.h"

#include <cstdio>

#include "color_converter.h"

__codec_begin

std::tuple<std::shared_ptr<uint8_t[]>, std::shared_ptr<uint8_t[]>, std::shared_ptr<uint8_t[]>> YUVUtil::Convert2YUV(uint32_t width, uint32_t height, uint8_t channel_num, const std::vector<uint8_t>& data)
{
	std::shared_ptr<uint8_t[]> y_data(new uint8_t[width * height]);
	std::shared_ptr<uint8_t[]> u_data(new uint8_t[width * height]);
	std::shared_ptr<uint8_t[]> v_data(new uint8_t[width * height]);

	auto y_data_ptr = y_data.get();
	auto u_data_ptr = u_data.get();
	auto v_data_ptr = v_data.get();

	for (int i = 0; i < width * height; ++i)
	{
		int r = data[i * channel_num];
		int g = data[i * channel_num + 1];
		int b = data[i * channel_num + 2];
		std::tie(*y_data_ptr++, *u_data_ptr++, *v_data_ptr++) = ColorConverter::RGB2YUV709Full(r, g, b);
	}

	return std::make_tuple(y_data, u_data, v_data);
}

bool YUVUtil::Serial(uint32_t width, uint32_t height, std::shared_ptr<uint8_t[]> y_data, std::shared_ptr<uint8_t[]> u_data, std::shared_ptr<uint8_t[]> v_data, const std::string& file_path)
{
	auto file_handle = fopen(file_path.c_str(), "wb");
	if (!file_handle)
		return false;

	fwrite(y_data.get(), 1, width * height, file_handle);
	fwrite(u_data.get(), 1, width * height, file_handle);
	fwrite(v_data.get(), 1, width * height, file_handle);

	fclose(file_handle);

	return true;
}

__codec_end
