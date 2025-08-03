#include "intra4_luma_flow.h"

#include "intra4_luma_flow_node.h"
#include "encoder_context.h"

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
		int cost8x8 = 6 * m_encoder_context->lambda + 0.4999;
		bool all_zero = true;
		for (uint8_t index_in_block4x4 = 0; index_in_block4x4 < 4; ++index_in_block4x4)
		{
			auto node = std::make_shared<Intra4LumaFlowNode>(m_mb, m_encoder_context, m_reconstructed_data, m_prediction_types, index_in_block8x8, index_in_block4x4);
			node->Frontend();
			cost8x8 += node->GetCost();
			m_nodes.push_back(node);

			all_zero &= node->IsAllZero();
		}
		m_cost += cost8x8;

		if (!all_zero)
			m_cbp |= 1 << index_in_block8x8;
	}
}

void Intra4LumaFlow::Backend()
{

}

std::vector<Intra4LumaPredictionType> Intra4LumaFlow::GetPredictionTypes() const
{
	return m_prediction_types;
}

__codec_end

