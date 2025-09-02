#include "inter_p16x16_luma_predictor.h"

#include "macroblock.h"
#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"

__codec_begin

InterP16x16LumaPredictor::InterP16x16LumaPredictor()
{
}

InterP16x16LumaPredictor::~InterP16x16LumaPredictor()
{
}

void InterP16x16LumaPredictor::Decide()
{
	auto pos = m_mb->GetPosition();
	m_predicted_data = DataUtil::ObtainDataInBlock(m_encoder_context->last_frame->y_data, pos.first, pos.second, 16, 16, m_encoder_context->width);


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