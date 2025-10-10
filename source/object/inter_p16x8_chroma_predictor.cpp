#include "inter_p16x8_chroma_predictor.h"

#include "macroblock.h"
#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"
#include "motion_info_context.h"
#include "math_util.h"
#include "interpolate_util.h"

__codec_begin

InterP16x8ChromaPredictor::InterP16x8ChromaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index) :
	m_mb(mb), m_encoder_context(encoder_context), m_segment_index(segment_index)
{
}

InterP16x8ChromaPredictor::~InterP16x8ChromaPredictor()
{
}

void InterP16x8ChromaPredictor::Decide()
{
	auto motion_info = m_encoder_context->motion_info_context->GetMotionInfo(m_mb->GetAddress(), 0, m_segment_index * 2);
	auto mv = motion_info.mv;
	auto pos = m_mb->GetPosition();
	pos.second += m_segment_index * 8;

	uint32_t new_x = MathUtil::Clamp<int>((pos.first << 2) + mv.x, 0, (m_encoder_context->width / 2 - 1) * 8);
	uint32_t new_y = MathUtil::Clamp<int>((pos.second << 2) + mv.y, 0, (m_encoder_context->height / 2 - 1) * 8);

	m_predicted_data[0].SetData(InterpolateUtil::InterpolateChromaBlock(m_encoder_context->last_frame->u_data, new_x, new_y, 8, 4, m_encoder_context->width / 2, m_encoder_context->height / 2));
	auto origin_block_data = m_mb->GetOriginalChromaBlockData8x4(PlaneType::Cb, m_segment_index);
	m_diff_data[0] = origin_block_data - m_predicted_data[0];

	m_predicted_data[1].SetData(InterpolateUtil::InterpolateChromaBlock(m_encoder_context->last_frame->v_data, new_x, new_y, 8, 4, m_encoder_context->width / 2, m_encoder_context->height / 2));
	origin_block_data = m_mb->GetOriginalChromaBlockData8x4(PlaneType::Cr, m_segment_index);
	m_diff_data[1] = origin_block_data - m_predicted_data[1];
}

BlockData<8, 4> InterP16x8ChromaPredictor::GetPredictedData(PlaneType plane_type) const
{
	return plane_type == PlaneType::Cb ? m_predicted_data[0] : m_predicted_data[1];
}

BlockData<8, 4, int32_t> InterP16x8ChromaPredictor::GetDiffData(PlaneType plane_type) const
{
	return plane_type == PlaneType::Cb ? m_diff_data[0] : m_diff_data[1];
}

__codec_end