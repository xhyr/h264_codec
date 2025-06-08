#include "image_util.h"

#include "image_reader.h"
#include "color_data.h"
#include "yuv_frame.h"

__codec_begin

std::shared_ptr<YUVFrame> ImageUtil::Read2YUVFrame(const std::string& file_path)
{
	auto color_data = ImageReader::ReadImage(file_path);
	if (!color_data || color_data->width % 2 || color_data->height % 2 || color_data->channel_num < 3) //just consider rgb/rgba pictures
		return nullptr;

	return std::make_shared<YUVFrame>(color_data);
}

__codec_end
