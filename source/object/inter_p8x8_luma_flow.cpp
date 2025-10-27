#include "inter_p8x8_luma_flow.h"

#include "inter_p8x8_luma_flow_node_base.h"
#include "bytes_data.h"
#include "segment_util.h"
#include "reconstruct_util.h"

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
	Predict();
	TransformAndQuantize();
	InverseQuantizeAndTransform();
	Reconstruct();

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

void InterP8x8LumaFlow::Predict()
{
	for (uint8_t i = 0; i < 4; ++i)
	{
		m_nodes.emplace_back(InterP8x8LumaFlowNodeBase::Create(MBType::SMB8x8, m_mb, m_encoder_context, i));
		m_nodes[i]->Predict();
		m_nodes[i]->FillDiffData(m_diff_datas);
	}
}

void InterP8x8LumaFlow::Reconstruct()
{
	for (uint8_t segment_index = 0; segment_index < 4; ++segment_index)
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
}

__codec_end