#include "inter_p8x16_chroma_predictor.h"

#include "macroblock.h"
#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"
#include "motion_info_context.h"
#include "math_util.h"
#include "interpolate_util.h"
#include "mb_util.h"

__codec_begin

InterP8x16ChromaPredictor::InterP8x16ChromaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index) :
	m_mb(mb), m_encoder_context(encoder_context), m_segment_index(segment_index)
{
}

InterP8x16ChromaPredictor::~InterP8x16ChromaPredictor()
{
}

void InterP8x16ChromaPredictor::Decide()
{
	auto motion_info = m_encoder_context->motion_info_context->GetMotionInfo(m_mb->GetAddress(), m_segment_index * 2, 0);
	auto mv = motion_info.mv;
	auto pos = m_mb->GetPosition();
	pos.first += m_segment_index * 8;

	uint32_t new_x = MathUtil::Clamp<int>((pos.first << 2) + mv.x, 0, (m_encoder_context->width / 2 - 1) * 8);
	uint32_t new_y = MathUtil::Clamp<int>((pos.second << 2) + mv.y, 0, (m_encoder_context->height / 2 - 1) * 8);

	m_predicted_data[0].SetData(InterpolateUtil::InterpolateChromaBlock(m_encoder_context->last_frame->u_data, new_x, new_y, 4, 8, m_encoder_context->width / 2, m_encoder_context->height / 2));
	auto origin_block_data = MBUtil::GetOriginalChromaBlockData<4, 8>(m_mb, PlaneType::Cb, m_segment_index); 
	m_diff_data[0] = origin_block_data - m_predicted_data[0];

	m_predicted_data[1].SetData(InterpolateUtil::InterpolateChromaBlock(m_encoder_context->last_frame->v_data, new_x, new_y, 4, 8, m_encoder_context->width / 2, m_encoder_context->height / 2));
	origin_block_data = MBUtil::GetOriginalChromaBlockData<4, 8>(m_mb, PlaneType::Cr, m_segment_index);
	m_diff_data[1] = origin_block_data - m_predicted_data[1];
}

BlockData<4, 8> InterP8x16ChromaPredictor::GetPredictedData(PlaneType plane_type) const
{
	return plane_type == PlaneType::Cb ? m_predicted_data[0] : m_predicted_data[1];
}

BlockData<4, 8, int32_t> InterP8x16ChromaPredictor::GetDiffData(PlaneType plane_type) const
{
	return plane_type == PlaneType::Cb ? m_diff_data[0] : m_diff_data[1];
}

__codec_end