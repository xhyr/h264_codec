#pragma once

#include "block_data.h"

__codec_begin

class Macroblock;
struct EncoderContext;
struct MotionInfo;
struct MotionVector;

class InterP8x8LumaFlowNodeBase
{
public:
	InterP8x8LumaFlowNodeBase(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index);
	virtual ~InterP8x8LumaFlowNodeBase();

	virtual void Predict() = 0;

	virtual std::vector<MotionInfo> GetMotionInfos() const = 0;

	virtual std::vector<MotionVector> GetMVDs() const = 0;

	BlockData<8, 8> GetPredictedData() const;

	BlockData<8, 8, int32_t> GetDiffData() const;

protected:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	uint8_t m_segment_index;

	BlockData<8, 8> m_predicted_data;
	BlockData<8, 8, int32_t> m_diff_data;
};

__codec_end