#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include "mb_type.h"
#include "block_data.h"
#include "common_types.h"

__codec_begin

class Macroblock;
struct EncoderContext;

class InterP16x16ChromaPredictor
{
public:
	InterP16x16ChromaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~InterP16x16ChromaPredictor();

	void Decide();

	BlockData<8, 8> GetPredictedData(PlaneType plane_type) const;

	BlockData<8, 8, int32_t> GetDiffData(PlaneType plane_type) const;

private:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;

	BlockData<8, 8> m_predicted_data[2];
	BlockData<8, 8, int32_t> m_diff_data[2];
};

__codec_end