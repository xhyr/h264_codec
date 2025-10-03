#include "inter_p16x16_chroma_predictor.h"

#include "macroblock.h"
#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"
#include "motion_info_context.h"
#include "math_util.h"
#include "interpolate_util.h"

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
	auto motion_info =  m_encoder_context->motion_info_context->GetMotionInfo(m_mb->GetAddress(), 0, 0);
	auto mv = motion_info.mv;
	auto pos = m_mb->GetPosition();

	uint32_t new_x = MathUtil::Clamp<int>((pos.first << 2) + mv.x, 0, (m_encoder_context->width / 2 - 1) * 8);
	uint32_t new_y = MathUtil::Clamp<int>((pos.second << 2) + mv.y, 0, (m_encoder_context->height / 2 - 1) * 8);
 
	m_predicted_data[0].SetData(InterpolateUtil::InterpolateChromaBlock(m_encoder_context->last_frame->u_data, new_x, new_y, 8, 8, m_encoder_context->width / 2, m_encoder_context->height / 2));
	auto origin_block_data = m_mb->GetOriginalChromaBlockData(PlaneType::Cb);
	m_diff_data[0] = origin_block_data - m_predicted_data[0];

	m_predicted_data[1].SetData(InterpolateUtil::InterpolateChromaBlock(m_encoder_context->last_frame->v_data, new_x, new_y, 8, 8, m_encoder_context->width / 2, m_encoder_context->height / 2));
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