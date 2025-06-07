#include "image_reader.h"

#include "color_data.h"

__codec_begin

std::shared_ptr<ColorData> ImageReader::ReadImage(const std::string& file_path, bool premultiply_alpha)
{
	return std::shared_ptr<ColorData>();
}

__codec_end


