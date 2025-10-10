#pragma once

#include <cstdint>
#include <memory>

#include "block_data.h"
#include "motion_info.h"

__codec_begin

class Macroblock;
struct EncoderContext;

class InterP16x8LumaPredictor
{
public:
	InterP16x8LumaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index);
	~InterP16x8LumaPredictor();

	void Decide();

	BlockData<16, 8> GetPredictedData() const;

	BlockData<16, 8, int32_t> GetDiffData() const;

	MotionInfo GetMotionInfo() const;

	MotionVector GetMVD() const;

private:
	void Init();

private:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	uint8_t m_segment_index;

	uint32_t m_x;
	uint32_t m_y;
	uint32_t m_x_in_block;
	uint32_t m_y_in_block;
	uint32_t m_width_in_block;
	uint32_t m_height_in_block;

	MotionInfo m_motion_info;
	BlockData<16, 8> m_predicted_data;
	BlockData<16, 8, int32_t> m_diff_data;
	MotionVector m_mvd;
};

__codec_end