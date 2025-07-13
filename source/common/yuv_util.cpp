#include "yuv_util.h"

#include <cstdio>

#include "color_converter.h"
#include "math_util.h"

__codec_begin

std::tuple<std::shared_ptr<uint8_t[]>, std::shared_ptr<uint8_t[]>, std::shared_ptr<uint8_t[]>> YUVUtil::Convert2YUV444P(uint32_t width, uint32_t height, uint8_t channel_num, const std::vector<uint8_t>& data)
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

std::tuple<std::shared_ptr<uint8_t[]>, std::shared_ptr<uint8_t[]>, std::shared_ptr<uint8_t[]>> YUVUtil::Convert2YUV420P(uint32_t width, uint32_t height, uint8_t channel_num, const std::vector<uint8_t>& data)
{
	auto [y_data_444, u_data_444, v_data_444] = Convert2YUV444P(width, height, channel_num, data);

	std::shared_ptr<uint8_t[]> y_data = y_data_444;
	std::shared_ptr<uint8_t[]> u_data(new uint8_t[width * height / 4]);
	std::shared_ptr<uint8_t[]> v_data(new uint8_t[width * height / 4]);

	auto u_data_ptr = u_data.get();
	auto v_data_ptr = v_data.get();

	for (uint32_t y = 0; y < height / 2; ++y)
	{
		for (uint32_t x = 0; x < width / 2; ++x)
		{
			int leftup_u = u_data_444.get()[y * 2 * width + x * 2];
			int leftdown_u = u_data_444.get()[(y * 2 + 1) * width + x * 2];
			int rightup_u = u_data_444.get()[y * 2 * width + x * 2 + 1];
			int rightdown_u = u_data_444.get()[(y * 2 + 1) * width + x * 2 + 1];
			*u_data_ptr++ = MathUtil::Average({ leftup_u, leftdown_u, rightup_u, rightdown_u });

			int leftup_v = v_data_444.get()[y * 2 * width + x * 2];
			int leftdown_v = v_data_444.get()[(y * 2 + 1) * width + x * 2];
			int rightup_v = v_data_444.get()[y * 2 * width + x * 2 + 1];
			int rightdown_v = v_data_444.get()[(y * 2 + 1) * width + x * 2 + 1];
			*v_data_ptr++ = MathUtil::Average({ leftup_v, leftdown_v, rightup_v, rightdown_v });
		}
	}

	return std::make_tuple(y_data, u_data, v_data);
}

bool YUVUtil::Serial(uint32_t width, uint32_t height, const std::string& file_path, std::shared_ptr<uint8_t[]> y_data, std::shared_ptr<uint8_t[]> u_data, std::shared_ptr<uint8_t[]> v_data)
{
	auto file_handle = fopen(file_path.c_str(), "wb");
	if (!file_handle)
		return false;

	fwrite(y_data.get(), 1, width * height, file_handle);
	fwrite(u_data.get(), 1, width * height / 4, file_handle);
	fwrite(v_data.get(), 1, width * height / 4, file_handle);

	fclose(file_handle);

	return true;
}

bool YUVUtil::Unserial(uint32_t width, uint32_t height, const std::string& file_path, std::shared_ptr<uint8_t[]>& y_data, std::shared_ptr<uint8_t[]>& u_data, std::shared_ptr<uint8_t[]>& v_data, uint32_t tick)
{
	auto file_handle = fopen(file_path.c_str(), "rb");
	if (!file_handle)
		return false;

	if (tick != 0)
		fseek(file_handle, tick * width * height * 3 / 2, SEEK_SET);

	y_data.reset(new uint8_t[width * height]);
	u_data.reset(new uint8_t[width * height / 4]);
	v_data.reset(new uint8_t[width * height / 4]);

    fread(y_data.get(), 1, width * height, file_handle);
	fread(u_data.get(), 1, width * height / 4, file_handle);
	fread(v_data.get(), 1, width * height / 4, file_handle);

	fclose(file_handle);

    return true;
}

__codec_end
