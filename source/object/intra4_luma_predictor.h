#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include "mb_type.h"
#include "prediction_type.h"
#include "block_data.h"

__codec_begin

class Macroblock;
struct EncoderContext;

class Intra4Predictor
{
public:
	Intra4Predictor(std::weak_ptr<Macroblock> macroblock, std::shared_ptr<EncoderContext> encoder_context);
	~Intra4Predictor();

	std::vector<Intra4LumaPredictionType> Decide();

	int GetCost() const;

private:
	Intra4LumaPredictionType Decide(uint8_t index_in_block8x8, uint8_t block_index);

	void ObtainNeighborBlocksInfo();

private:
	std::weak_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;

	
};

__codec_end