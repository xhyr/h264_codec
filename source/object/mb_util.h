#pragma once

#include <cstdint>
#include <memory>

#include "prediction_type.h"

__codec_begin

struct EncoderContext;

struct MBUtil
{
	static bool IsAvailable(uint32_t mb_addr, std::shared_ptr<EncoderContext> encoder_context);

	static int CalculateIntra16Offset(uint8_t cbp, Intra16LumaPredictionType prediction_type);

};

__codec_end