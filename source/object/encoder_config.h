#pragma once

#include <cstdint>
#include <string>

#include "global_defines.h"

__codec_begin

struct EncoderConfig
{
	std::string input_file_path;
	std::string output_file_path;

	uint32_t width;
	uint32_t height;

	uint32_t frames_to_encode;

	uint32_t profile_idc;
	uint32_t level_idc;
	uint32_t ref_frame_number;
};

__codec_end