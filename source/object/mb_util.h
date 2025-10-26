#pragma once

#include <cstdint>
#include <memory>

#include "prediction_type.h"
#include "block_data.h"
#include "segment_util.h"
#include "macroblock.h"

__codec_begin

struct EncoderContext;

struct MBUtil
{
	static bool IsAvailable(uint32_t mb_addr, std::shared_ptr<EncoderContext> encoder_context);

	static int CalculateIntra16Offset(uint8_t cbp, Intra16LumaPredictionType prediction_type);

	static bool IsIntra(std::shared_ptr<Macroblock> mb);

	static void FillDetailedCBP(bool all_zero, uint32_t block_8x8, uint32_t block_4x4, uint32_t& detailed_cbp);

	static void FillDetailedCBP(bool all_zero, uint32_t block_index, uint32_t& detailed_cbp);

	static void ResetDetailedCBP(uint32_t block_8x8, uint32_t& detailed_cbp);

	static bool IsLumaAllZero(uint32_t detailed_cbp, uint32_t x_in_block, uint32_t y_in_block);

	static std::pair<uint32_t, uint32_t> GetPositionInBlock(std::shared_ptr<Macroblock> mb, uint8_t block_8x8_segment_index);

	static BlockData<4, 4> GetOriginalLumaBlockData4x4(std::shared_ptr<Macroblock> mb, uint32_t x_in_block, uint32_t y_in_block);
	
	template<size_t block_width, size_t block_height>
	static BlockData<block_width, block_height> GetOriginalLumaBlockData(std::shared_ptr<Macroblock> mb, uint8_t segment_index)
	{
		auto [start_x, start_y] = SegmentUtil::GetLumaDataRect<block_width, block_height>(segment_index);
		return mb->GetOriginalLumaBlockData().GetBlock<block_width, block_height>(start_x, start_y);
	}

	template<size_t block_width, size_t block_height>
	static BlockData<block_width, block_height> GetOriginalLumaBlockData(std::shared_ptr<Macroblock> mb, uint8_t segment_index, uint8_t sub_segment_index)
	{
		auto [start_x, start_y] = SegmentUtil::GetLumaDataRect<block_width, block_height>(segment_index, sub_segment_index);
		return mb->GetOriginalLumaBlockData().GetBlock<block_width, block_height>(start_x, start_y);
	}

	template<size_t block_width, size_t block_height>
	static BlockData<block_width, block_height> GetOriginalChromaBlockData(std::shared_ptr<Macroblock> mb, PlaneType plane_type, uint8_t segment_index)
	{
		auto [start_x, start_y] = SegmentUtil::GetChromaDataRect<block_width, block_height>(segment_index);
		return mb->GetOriginalChromaBlockData(plane_type).GetBlock<block_width, block_height>(start_x, start_y);
	}
	
};

__codec_end