#include "yuv_frame.h"

#include "color_data.h"
#include "yuv_util.h"

__codec_begin

YUVFrame::YUVFrame():width(0), height(0)
{
}

YUVFrame::YUVFrame(std::shared_ptr<ColorData> color_data) : width(color_data->width), height(color_data->height)
{
	std::tie(y_data, u_data, v_data) = YUVUtil::Convert2YUV420P(width, height, color_data->channel_num, color_data->data);
}

YUVFrame::YUVFrame(uint32_t _width, uint32_t _height, const std::string& file_path, uint32_t tick)
{
	width = _width;
	height = _height;
	Unserial(width, height, file_path, tick);
}

YUVFrame::YUVFrame(uint32_t _width, uint32_t _height)
{
	width = _width;
	height = _height;
	y_data.reset(new uint8_t[width * height]);
	u_data.reset(new uint8_t[width * height / 4]);
	v_data.reset(new uint8_t[width * height / 4]);
}

bool YUVFrame::Serial(const std::string& file_path)
{
	return YUVUtil::Serial(width, height, file_path, y_data, u_data, v_data);
}

bool YUVFrame::Unserial(uint32_t width, uint32_t height, const std::string& file_path, uint32_t tick)
{
    return YUVUtil::Unserial(width, height, file_path, y_data, u_data, v_data, tick);
}

__codec_end