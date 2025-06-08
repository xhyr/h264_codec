#include "image_reader.h"

#include "color_data.h"
#include "file_util.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

__codec_begin

std::shared_ptr<ColorData> ImageReader::ReadImage(const std::string& file_path, bool premultiply_alpha)
{
	if (!FileUtil::IsFileExist(file_path))
		return nullptr;

	std::shared_ptr<ColorData> color_data;
	int width, height, channel_num;
	unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &channel_num, 0);
	if (data)
		color_data = std::make_shared<ColorData>(width, height, channel_num, data);

	return color_data;
}

__codec_end


