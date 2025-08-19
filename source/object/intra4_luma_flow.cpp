#include "intra4_luma_flow.h"

#include "intra4_luma_flow_node.h"
#include "encoder_context.h"
#include "bytes_data.h"
#include "coding_util.h"
#include "macroblock.h"

__codec_begin

Intra4LumaFlow::Intra4LumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	IntraLumaFlowBase(mb, encoder_context)
{

}

Intra4LumaFlow::~Intra4LumaFlow()
{
}

void Intra4LumaFlow::Frontend() 
{
	m_nodes.reserve(16);
	m_prediction_types.resize(16);
	for (uint8_t index_in_block8x8 = 0; index_in_block8x8 < 4; ++index_in_block8x8)
	{
		bool all_zero = true;
		for (uint8_t index_in_block4x4 = 0; index_in_block4x4 < 4; ++index_in_block4x4)
		{
			auto node = std::make_shared<Intra4LumaFlowNode>(m_mb, m_encoder_context, m_reconstructed_data, m_prediction_types, index_in_block8x8, index_in_block4x4);
			node->Frontend();
			m_nodes.push_back(node);
			std::shared_ptr<BytesData> tmp_bytes_data = std::make_shared<BytesData>();
			node->OutputCoefficients(tmp_bytes_data);

			all_zero &= node->IsAllZero();
		}

		if (!all_zero)
			m_cbp |= 1 << index_in_block8x8;
	}

	CalculateDistortion();
}

void Intra4LumaFlow::OutputPredictionTypes(std::shared_ptr<BytesData> bytes_data)
{
	for (const auto& node : m_nodes)
	{
		uint32_t most_probable_prediction_type = static_cast<uint32_t>(node->GetMostProbablePredictionType());
		uint32_t prediction_type = static_cast<uint32_t>(node->GetPredictionType());

		if (most_probable_prediction_type == prediction_type)
			CodingUtil::U_1(1, bytes_data);
		else
		{
			CodingUtil::U_1(0, bytes_data);
			CodingUtil::U_V(3, prediction_type < most_probable_prediction_type ? prediction_type : prediction_type - 1, bytes_data);
		}
	}
}

uint32_t Intra4LumaFlow::OutputCoefficients(std::shared_ptr<BytesData> bytes_data)
{
	uint32_t bit_count = 0;
	for (uint8_t index_in_block8x8 = 0; index_in_block8x8 < 4; ++index_in_block8x8)
	{
		if ((m_cbp & (1 << index_in_block8x8)) == 0)
			continue;

		for (uint8_t index_in_block4x4 = 0; index_in_block4x4 < 4; ++index_in_block4x4)
		{
			auto node = m_nodes[index_in_block8x8 * 4 + index_in_block4x4];
			bit_count += node->OutputCoefficients(bytes_data);
		}
	}
	return bit_count;
}

std::vector<Intra4LumaPredictionType> Intra4LumaFlow::GetPredictionTypes() const
{
	return m_prediction_types;
}

__codec_end

