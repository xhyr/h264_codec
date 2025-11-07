#include "inter_p4x4_chroma_predictor.h"

#include "macroblock.h"
#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"
#include "motion_info_context.h"
#include "math_util.h"
#include "interpolate_util.h"
#include "mb_util.h"

__codec_begin

InterP4x4ChromaPredictor::InterP4x4ChromaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index) :
	m_mb(mb), m_encoder_context(encoder_context), m_segment_index(segment_index)
{
}

InterP4x4ChromaPredictor::~InterP4x4ChromaPredictor()
{
}

void InterP4x4ChromaPredictor::Decide()
{
	auto motion_info = m_encoder_context->motion_info_context->GetMotionInfo(m_mb->GetAddress(), m_segment_index % 2 * 2, m_segment_index / 2 * 2);
	auto mv = motion_info.mv;
	auto pos = m_mb->GetPosition();
	pos.first += m_segment_index % 2 * 8;
	pos.second += m_segment_index / 2 * 8;

	uint32_t new_x = MathUtil::Clamp<int>((pos.first << 2) + mv.x, 0, (m_encoder_context->width / 2 - 1) * 8);
	uint32_t new_y = MathUtil::Clamp<int>((pos.second << 2) + mv.y, 0, (m_encoder_context->height / 2 - 1) * 8);

	m_predicted_data[0].SetData(InterpolateUtil::InterpolateChromaBlock(m_encoder_context->last_frame->u_data, new_x, new_y, 4, 4, m_encoder_context->width / 2, m_encoder_context->height / 2));
	auto origin_block_data = MBUtil::GetOriginalChromaBlockData<4, 4>(m_mb, PlaneType::Cb, m_segment_index); 
	m_diff_data[0] = origin_block_data - m_predicted_data[0];

	m_predicted_data[1].SetData(InterpolateUtil::InterpolateChromaBlock(m_encoder_context->last_frame->v_data, new_x, new_y, 4, 4, m_encoder_context->width / 2, m_encoder_context->height / 2));
	origin_block_data = MBUtil::GetOriginalChromaBlockData<4, 4>(m_mb, PlaneType::Cr, m_segment_index);
	m_diff_data[1] = origin_block_data - m_predicted_data[1];
}

void InterP4x4ChromaPredictor::FillDiffData(PlaneType plane_type, std::vector<BlockData<4, 4, int32_t>>& diff_datas)
{
	diff_datas.resize(4);
	diff_datas[m_segment_index] = GetDiffData(plane_type);
}

void InterP4x4ChromaPredictor::FillPredictedData(PlaneType plane_type, BlockData<8, 8>& predicted_data)
{
	predicted_data.SetBlock4x4(m_segment_index % 2, m_segment_index / 2, GetPredictedData(plane_type));
}

BlockData<4, 4> InterP4x4ChromaPredictor::GetPredictedData(PlaneType plane_type) const
{
	return plane_type == PlaneType::Cb ? m_predicted_data[0] : m_predicted_data[1];
}

BlockData<4, 4, int32_t> InterP4x4ChromaPredictor::GetDiffData(PlaneType plane_type) const
{
	return plane_type == PlaneType::Cb ? m_diff_data[0] : m_diff_data[1];
}

__codec_end