#pragma once

#include <cstdint>
#include <memory>

#include "prediction_type.h"

__codec_begin

struct EncoderContext;
class Macroblock;

struct MBUtil
{
	static bool IsAvailable(uint32_t mb_addr, std::shared_ptr<EncoderContext> encoder_context);

	static int CalculateIntra16Offset(uint8_t cbp, Intra16LumaPredictionType prediction_type);

	static bool IsIntra(std::shared_ptr<Macroblock> mb);

	static void FillDetailedCBP(bool all_zero, uint32_t block_8x8, uint32_t block_4x4, uint32_t& detailed_cbp);

	static void FillDetailedCBP(bool all_zero, uint32_t block_index, uint32_t& detailed_cbp);

	static void ResetDetailedCBP(uint32_t block_8x8, uint32_t& detailed_cbp);

	static bool IsLumaAllZero(uint32_t detailed_cbp, uint32_t x_in_block, uint32_t y_in_block);
};

__codec_end