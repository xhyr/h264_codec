#pragma once

#include "block_data.h"
#include "mb_type.h"

__codec_begin

class Macroblock;
struct EncoderContext;
class BytesData;

class InterP8x8LumaFlowNodeBase
{
public:
	InterP8x8LumaFlowNodeBase(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index);
	virtual ~InterP8x8LumaFlowNodeBase();

	virtual void Predict() = 0;

	virtual void FillDiffData(std::vector<BlockData<4, 4, int32_t>>& diff_datas) const = 0;

	virtual void UpdateMotionInfo() = 0;

	virtual uint32_t OutputMotionInfos(std::shared_ptr<BytesData> bytes_data) const = 0;

	BlockData<8, 8> GetPredictedData() const;

	static std::shared_ptr<InterP8x8LumaFlowNodeBase> Create(MBType mb_type, std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index);

protected:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	uint8_t m_segment_index;
	BlockData<8, 8> m_predicted_data;
};

__codec_end