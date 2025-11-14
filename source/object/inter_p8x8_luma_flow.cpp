#include "inter_p8x8_luma_flow.h"

#include "inter_p8x8_luma_flow_node_base.h"
#include "bytes_data.h"
#include "segment_util.h"
#include "reconstruct_util.h"
#include "mb_util.h"
#include "cost_util.h"

__codec_begin

InterP8x8LumaFlow::InterP8x8LumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	InterLumaFlowBase(mb, encoder_context)
{

}

InterP8x8LumaFlow::~InterP8x8LumaFlow()
{
}

void InterP8x8LumaFlow::Frontend()
{
	for (uint8_t segment_index = 0; segment_index < 4; ++segment_index)
	{
		int64_t min_cost = std::numeric_limits<int64_t>::max();
		MBType best_sub_mb_type = MBType::SMB8x8;

		for (auto sub_mb_type : { MBType::SMB8x8, MBType::SMB8x4, MBType::SMB4x8, MBType::SMB4x4 })
		{
			auto node = InterP8x8LumaFlowNodeBase::Create(sub_mb_type, m_mb, m_encoder_context, segment_index);
			node->Predict();
			node->FillDiffData(m_diff_datas);
			TransformAndQuantize(segment_index);
			InverseQuantizeAndTransform(segment_index);
			Reconstruct(segment_index);

			auto distortion = CalculateBlock8x8Distortion(segment_index);
			if (distortion > min_cost)
				continue;

			int64_t rate = node->OutputSubMBTypes(nullptr);
			rate += node->OutputMotionInfos(nullptr);
			if (m_cbp & (1 << segment_index))
			{
				auto bytes_data = std::make_shared<BytesData>();
				rate += OutputCoefficients(segment_index, bytes_data);
			}
				
			auto cost = CostUtil::CalculateRDCostMotion(distortion, rate, m_encoder_context);
			if (cost < min_cost)
			{
				min_cost = cost;
				best_sub_mb_type = sub_mb_type;
			}
		}

		auto node = InterP8x8LumaFlowNodeBase::Create(best_sub_mb_type, m_mb, m_encoder_context, segment_index);
		node->Predict();
		node->FillDiffData(m_diff_datas);
		TransformAndQuantize(segment_index);
		InverseQuantizeAndTransform(segment_index);
		Reconstruct(segment_index);
		m_nodes[segment_index] = node;
	}

	CalculateDistortion();
}

void InterP8x8LumaFlow::Backend()
{
	for (auto node : m_nodes)
		node->UpdateMotionInfo();
}

uint32_t InterP8x8LumaFlow::OutputSubMBTypes(std::shared_ptr<BytesData> bytes_data)
{
	auto total_bits = 0;
	for (auto node : m_nodes)
		total_bits += node->OutputSubMBTypes(bytes_data);
	return total_bits;
}

uint32_t InterP8x8LumaFlow::OutputMotionInfo(std::shared_ptr<BytesData> bytes_data)
{
	auto start_bit_count = bytes_data->GetBitsCount();

	for (auto node : m_nodes)
		node->OutputMotionInfos(bytes_data);

	auto finish_bit_count = bytes_data->GetBitsCount();
	return finish_bit_count - start_bit_count;
}

void InterP8x8LumaFlow::Reconstruct(uint8_t segment_index)
{
	auto predicted_data = m_nodes[segment_index]->GetPredictedData();
	auto [start_x, start_y] = SegmentUtil::GetLumaDataRect<8, 8>(segment_index);
	for (uint32_t y = start_y; y < start_y + 8; ++y)
	{
		for (uint32_t x = start_x; x < start_x + 8; ++x)
		{
			auto predicted_val = predicted_data.GetElement(x - start_x, y - start_y);
			auto residual_val = m_cbp == 0 ? 0 : m_diff_data.GetElement(x, y);
			auto val = ReconstructUtil::Reconstruct(predicted_val, residual_val);
			m_reconstructed_data.SetElement(x, y, val);
		}
	}
}

int64_t InterP8x8LumaFlow::CalculateBlock8x8Distortion(uint32_t block_8x8)
{
	auto original_block_data = MBUtil::GetOriginalLumaBlockData<8, 8>(m_mb, block_8x8);
	auto reconstruct_block_data = m_reconstructed_data.GetBlock<8, 8, int32_t>(block_8x8 % 2 * 8, block_8x8 / 2 * 8);
	return CostUtil::CalculateSSEDistortion(reconstruct_block_data - original_block_data);
}

__codec_end