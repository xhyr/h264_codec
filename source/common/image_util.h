#pragma once

#include <memory>
#include <string>

#include "global_defines.h"

__codec_begin

struct YUVFrame;

struct ImageUtil
{
	static std::shared_ptr<YUVFrame> Read2YUVFrame(const std::string& file_path);


};

__codec_end