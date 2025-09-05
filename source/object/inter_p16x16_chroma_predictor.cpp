#include "inter_p16x16_chroma_predictor.h"

#include "macroblock.h"
#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"

__codec_begin

InterP16x16ChromaPredictor::InterP16x16ChromaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	m_mb(mb), m_encoder_context(encoder_context)
{
}

InterP16x16ChromaPredictor::~InterP16x16ChromaPredictor()
{
}

void InterP16x16ChromaPredictor::Decide()
{
	auto pos = m_mb->GetPosition();
	pos.first /= 2;
	pos.second /= 2;

	m_predicted_data[0].SetData(DataUtil::ObtainDataInBlock(m_encoder_context->last_frame->u_data, pos.first, pos.second, 8, 8, m_encoder_context->width / 2));
	auto origin_block_data = m_mb->GetOriginalChromaBlockData(PlaneType::Cb);
	m_diff_data[0] = origin_block_data - m_predicted_data[0];

	m_predicted_data[1].SetData(DataUtil::ObtainDataInBlock(m_encoder_context->last_frame->v_data, pos.first, pos.second, 8, 8, m_encoder_context->width / 2));
	origin_block_data = m_mb->GetOriginalChromaBlockData(PlaneType::Cr);
	m_diff_data[1] = origin_block_data - m_predicted_data[1];
}

BlockData<8, 8> InterP16x16ChromaPredictor::GetPredictedData(PlaneType plane_type) const
{
	return plane_type == PlaneType::Cb ? m_predicted_data[0] : m_predicted_data[1];
}

BlockData<8, 8, int32_t> InterP16x16ChromaPredictor::GetDiffData(PlaneType plane_type) const
{
	return plane_type == PlaneType::Cb ? m_diff_data[0] : m_diff_data[1];
}

__codec_end