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

class InterP8x16ChromaPredictor
{
public:
	InterP8x16ChromaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index);
	~InterP8x16ChromaPredictor();

	void Decide();

	BlockData<4, 8> GetPredictedData(PlaneType plane_type) const;

	BlockData<4, 8, int32_t> GetDiffData(PlaneType plane_type) const;

private:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	uint8_t m_segment_index;

	BlockData<4, 8> m_predicted_data[2];
	BlockData<4, 8, int32_t> m_diff_data[2];
};

__codec_end