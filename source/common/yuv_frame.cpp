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

bool YUVFrame::Serial(const std::string& file_path)
{
	return YUVUtil::Serial(width, height, y_data, u_data, v_data, file_path);
}

__codec_end