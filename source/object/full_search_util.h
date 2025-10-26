#pragma once

#include <memory>

#include "global_defines.h"

__codec_begin

struct MotionVector;
struct SearchInfo;
struct EncoderContext;

struct FullSearchUtil
{
	static MotionVector FindBestMV(const SearchInfo& search_info, std::shared_ptr<EncoderContext> encoder_context, MotionVector& mvd);

};

__codec_end