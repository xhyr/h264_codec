#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "global_defines.h"

__codec_begin

struct ColorData;

struct ImageReader
{
	static std::shared_ptr<ColorData> ReadImage(const std::string& file_path, bool premultiply_alpha = true);
};

__codec_end