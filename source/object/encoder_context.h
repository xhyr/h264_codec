#pragma once

#include <cstdint>
#include <memory>

#include "global_defines.h"

__codec_begin

struct YUVFrame;
struct EncoderConfig;

struct EncoderContext
{
	uint32_t width;
	uint32_t height;
	uint32_t width_in_mb;
	uint32_t height_in_mb;
	uint32_t mb_num;
	std::shared_ptr<YUVFrame> yuv_frame;

	std::shared_ptr<EncoderConfig> config;
};

__codec_end