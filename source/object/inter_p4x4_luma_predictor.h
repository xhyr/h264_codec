#pragma once

#include <cstdint>
#include <memory>

#include "block_data.h"
#include "motion_info.h"

__codec_begin

class Macroblock;
struct EncoderContext;

class InterP4x4LumaPredictor
{
public:
	InterP4x4LumaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index, uint8_t sub_segment_index);
	~InterP4x4LumaPredictor();

	void Decide();

	BlockData<4, 4> GetPredictedData() const;

	MotionVector GetMVD() const;

	void FillDiffData(std::vector<BlockData<4, 4, int32_t>>& diff_datas) const;

	void UpdateMotionInfo();
	
	void FillPredictedData(BlockData<8, 8>& predicted_data) const;

private:
	void Init();

private:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	uint8_t m_segment_index;
	uint8_t m_sub_segment_index;

	uint32_t m_x;
	uint32_t m_y;
	uint32_t m_x_in_block;
	uint32_t m_y_in_block;
	uint32_t m_x_in_block_mb;
	uint32_t m_y_in_block_mb;
	uint32_t m_width_in_block;
	uint32_t m_height_in_block;

	MotionInfo m_motion_info;
	BlockData<4, 4> m_predicted_data;
	BlockData<4, 4, int32_t> m_diff_data;
	MotionVector m_mvd;
};

__codec_end