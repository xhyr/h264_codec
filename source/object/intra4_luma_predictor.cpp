#include "intra4_luma_predictor.h"

#include "block_neighbors.h"

__codec_begin

Intra4Predictor::Intra4Predictor(std::weak_ptr<Macroblock> macroblock, std::shared_ptr<EncoderContext> encoder_context) :
	m_mb(macroblock), m_encoder_context(encoder_context)
{
}

Intra4Predictor::~Intra4Predictor()
{
}

std::vector<Intra4LumaPredictionType> Intra4Predictor::Decide()
{
	std::vector<Intra4LumaPredictionType> prediction_types;
	prediction_types.reserve(16);
	for (uint8_t index_in_block8x8 = 0; index_in_block8x8 < 4; ++index_in_block8x8)
	{
		for (uint8_t block_index = 0; block_index < 4; ++block_index)
		{
			auto prediction_type = Decide(index_in_block8x8, block_index);
			prediction_types.push_back(prediction_type);
		}
	}
	return {};
}

int Intra4Predictor::GetCost() const
{
	return 0;
}

Intra4LumaPredictionType Intra4Predictor::Decide(uint8_t index_in_block8x8, uint8_t block_index)
{
	uint32_t x_in_block = (index_in_block8x8 % 2) * 2 + (block_index % 2);
	uint32_t y_in_block = (index_in_block8x8 / 2) * 2 + (block_index / 2);

	BlockNeighbors block_neighbors(m_mb.lock(), x_in_block, y_in_block);
	auto edge_data = block_neighbors.GetEdgeData();
	


	return Intra4LumaPredictionType::DC;
}

void Intra4Predictor::ObtainNeighborBlocksInfo()
{

}

__codec_end
