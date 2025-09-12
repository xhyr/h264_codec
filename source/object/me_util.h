#pragma once

#include <cstdint>
#include <memory>

#include "global_defines.h"

__codec_begin

struct MotionVector;
struct EncoderContext;

struct MEUtil
{
	static MotionVector GetPredictorMV();
};

__codec_end