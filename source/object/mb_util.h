#pragma once

#include <cstdint>
#include <memory>

#include "global_defines.h"

__codec_begin

struct EncoderContext;

struct MBUtil
{
	static bool IsAvailable(uint32_t mb_addr, std::shared_ptr<EncoderContext> encoder_context);



};

__codec_end