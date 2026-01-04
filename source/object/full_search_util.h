#pragma once

#include <memory>

#include "global_defines.h"

__codec_begin

struct MotionVector;
struct SearchInfo;
struct EncoderContext;
struct MotionInfo;

struct FullSearchUtil
{
	static MotionInfo FindBestMotionInfo(const SearchInfo& search_info, std::shared_ptr<EncoderContext> encoder_context, MotionVector& mvd);

};

__codec_end