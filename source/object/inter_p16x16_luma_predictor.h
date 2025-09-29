#pragma once

#include <cstdint>
#include <memory>

#include "block_data.h"
#include "motion_info.h"

__codec_begin

class Macroblock;
struct EncoderContext;

class InterP16x16LumaPredictor
{
public:
	InterP16x16LumaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~InterP16x16LumaPredictor();

	void Decide();

	BlockData<16, 16> GetPredictedData() const;

	BlockData<16, 16, int32_t> GetDiffData() const;

	MotionInfo GetMotionInfo() const;

	MotionVector GetMVD() const;

private:
	void Init();

private:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;

	uint32_t m_x;
	uint32_t m_y;
	uint32_t m_x_in_block;
	uint32_t m_y_in_block;
	uint32_t m_width_in_block;
	uint32_t m_height_in_block;

	MotionInfo m_motion_info;
	BlockData<16, 16> m_predicted_data;
	BlockData<16, 16, int32_t> m_diff_data;
	MotionVector m_mvd;
};

__codec_end