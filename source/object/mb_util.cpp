#include "mb_util.h"

#include "encoder_context.h"
#include "bit_util.h"

__codec_begin

bool MBUtil::IsAvailable(uint32_t mb_addr, std::shared_ptr<EncoderContext> encoder_context)
{
	return mb_addr < encoder_context->mb_num;
}

int MBUtil::CalculateIntra16Offset(uint8_t cbp, Intra16LumaPredictionType prediction_type)
{
    return (cbp & 15 ? 13 : 1) + (int)prediction_type + ((cbp & 0x30) >> 2);
}

bool MBUtil::IsIntra(std::shared_ptr<Macroblock> mb)
{
	auto mb_type = mb->GetType();
	return mb_type == MBType::I4 || mb_type == MBType::I16;
}

void MBUtil::FillDetailedCBP(bool all_zero, uint32_t block_8x8, uint32_t block_4x4, uint32_t& detailed_cbp)
{
	uint32_t x_in_block = block_8x8 % 2 * 2 + block_4x4 % 2;
	uint32_t y_in_block = block_8x8 / 2 * 2 + block_4x4 / 2;
	uint32_t block_index = y_in_block * 4 + x_in_block;
	BitUtil::SetValue(detailed_cbp, block_index, all_zero ? 0 : 1);
}

void MBUtil::FillDetailedCBP(bool all_zero, uint32_t block_index, uint32_t& detailed_cbp)
{
	BitUtil::SetValue(detailed_cbp, block_index, all_zero ? 0 : 1);
}

void MBUtil::ResetDetailedCBP(uint32_t block_8x8, uint32_t& detailed_cbp)
{
	for (uint32_t block_4x4 = 0; block_4x4 < 4; ++block_4x4)
		FillDetailedCBP(true, block_8x8, block_4x4, detailed_cbp);
}

bool MBUtil::IsLumaAllZero(uint32_t detailed_cbp, uint32_t x_in_block, uint32_t y_in_block)
{
	uint32_t block_index = y_in_block * 4 + x_in_block;
	return 0 == (detailed_cbp & (1 << block_index));
}

std::pair<uint32_t, uint32_t> MBUtil::GetPositionInBlock(std::shared_ptr<Macroblock> mb, uint8_t block_8x8_segment_index)
{
	auto pos = mb->GetPosition();
	uint32_t x_in_block = pos.first / 4 + (block_8x8_segment_index % 2) * 2;
	uint32_t y_in_block = pos.second / 4 + (block_8x8_segment_index / 2) * 2;
	return std::make_pair(x_in_block, y_in_block);
}

BlockData<4, 4> MBUtil::GetOriginalLumaBlockData4x4(std::shared_ptr<Macroblock> mb, uint32_t x_in_block, uint32_t y_in_block)
{
	return mb->GetOriginalLumaBlockData().GetBlock4x4<uint8_t>(x_in_block, y_in_block);
}

__codec_end


