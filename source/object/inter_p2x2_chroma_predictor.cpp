#include "inter_p2x2_chroma_predictor.h"

#include "macroblock.h"
#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"
#include "motion_info_context.h"
#include "math_util.h"
#include "interpolate_util.h"
#include "mb_util.h"
#include "dpb.h"

__codec_begin

InterP2x2ChromaPredictor::InterP2x2ChromaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index, uint8_t sub_segment_index, bool skip) :
	m_mb(mb), m_encoder_context(encoder_context), m_segment_index(segment_index), m_sub_segment_index(sub_segment_index), m_skip(skip)
{
}

InterP2x2ChromaPredictor::~InterP2x2ChromaPredictor()
{
}

void InterP2x2ChromaPredictor::Decide()
{
	auto motion_info = m_encoder_context->motion_info_context->GetMotionInfo(m_mb->GetAddress(), m_segment_index % 2 * 2 + m_sub_segment_index % 2, m_segment_index / 2 * 2 + m_sub_segment_index / 2);
	auto mv = motion_info.mv;
	auto ref_id = motion_info.ref_id;
	auto pos = m_mb->GetPosition();
	pos.first += m_segment_index % 2 * 8 + m_sub_segment_index % 2 * 4;
	pos.second += m_segment_index / 2 * 8 + m_sub_segment_index / 2 * 4;

	uint32_t new_x = MathUtil::Clamp<int>((pos.first << 2) + mv.x, 0, (m_encoder_context->width / 2 - 1) * 8);
	uint32_t new_y = MathUtil::Clamp<int>((pos.second << 2) + mv.y, 0, (m_encoder_context->height / 2 - 1) * 8);

	m_predicted_data[0].SetData(InterpolateUtil::InterpolateChromaBlock(m_encoder_context->dpb->GetFrame(ref_id)->u_data, new_x, new_y, 2, 2, m_encoder_context->width / 2, m_encoder_context->height / 2));
	m_predicted_data[1].SetData(InterpolateUtil::InterpolateChromaBlock(m_encoder_context->dpb->GetFrame(ref_id)->v_data, new_x, new_y, 2, 2, m_encoder_context->width / 2, m_encoder_context->height / 2));
	if (!m_skip)
	{
		auto origin_block_data = MBUtil::GetOriginalChromaBlockData<2, 2>(m_mb, PlaneType::Cb, m_segment_index, m_sub_segment_index);
		m_diff_data[0] = origin_block_data - m_predicted_data[0];
		origin_block_data = MBUtil::GetOriginalChromaBlockData<2, 2>(m_mb, PlaneType::Cr, m_segment_index, m_sub_segment_index);
		m_diff_data[1] = origin_block_data - m_predicted_data[1];
	}
}

void InterP2x2ChromaPredictor::FillDiffData(PlaneType plane_type, std::vector<BlockData<4, 4, int32_t>>& diff_datas)
{
	diff_datas.resize(4);
	if (m_skip)
	{
		for (auto& diff_data : diff_datas)
			diff_data.Reset(0);
	}
	else DataUtil::SetData(diff_datas[m_segment_index], GetDiffData(plane_type), m_sub_segment_index);
}

void InterP2x2ChromaPredictor::FillPredictedData(PlaneType plane_type, BlockData<8, 8>& predicted_data)
{
	DataUtil::SetData(predicted_data, GetPredictedData(plane_type), m_segment_index, m_sub_segment_index);
}

BlockData<2, 2> InterP2x2ChromaPredictor::GetPredictedData(PlaneType plane_type) const
{
	return plane_type == PlaneType::Cb ? m_predicted_data[0] : m_predicted_data[1];
}

BlockData<2, 2, int32_t> InterP2x2ChromaPredictor::GetDiffData(PlaneType plane_type) const
{
	return plane_type == PlaneType::Cb ? m_diff_data[0] : m_diff_data[1];
}

__codec_end