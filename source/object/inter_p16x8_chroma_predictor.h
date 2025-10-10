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

class InterP16x8ChromaPredictor
{
public:
	InterP16x8ChromaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index);
	~InterP16x8ChromaPredictor();

	void Decide();

	BlockData<8, 4> GetPredictedData(PlaneType plane_type) const;

	BlockData<8, 4, int32_t> GetDiffData(PlaneType plane_type) const;

private:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	uint8_t m_segment_index;

	BlockData<8, 4> m_predicted_data[2];
	BlockData<8, 4, int32_t> m_diff_data[2];
};

__codec_end