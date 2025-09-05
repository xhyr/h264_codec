#include "inter_p16x16_luma_predictor.h"

#include "macroblock.h"
#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"

__codec_begin

InterP16x16LumaPredictor::InterP16x16LumaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	m_mb(mb), m_encoder_context(encoder_context)
{
}

InterP16x16LumaPredictor::~InterP16x16LumaPredictor()
{
}

void InterP16x16LumaPredictor::Decide()
{
	auto pos = m_mb->GetPosition();
	m_predicted_data.SetData(DataUtil::ObtainDataInBlock(m_encoder_context->last_frame->y_data, pos.first, pos.second, 16, 16, m_encoder_context->width));
	auto origin_block_data = m_mb->GetOriginalLumaBlockData16x16();
	m_diff_data = origin_block_data - m_predicted_data;
}

BlockData<16, 16> InterP16x16LumaPredictor::GetPredictedData() const
{
	return m_predicted_data;
}

BlockData<16, 16, int32_t> InterP16x16LumaPredictor::GetDiffData() const
{
	return m_diff_data;
}

__codec_end