#pragma once

#include <cstdint>
#include <memory>

#include "block_data.h"

__codec_begin

class Macroblock;
struct EncoderContext;

class InterP16x16LumaPredictor
{
public:
	InterP16x16LumaPredictor();
	~InterP16x16LumaPredictor();

	void Decide();

	BlockData<16, 16> GetPredictedData() const;

	BlockData<16, 16, int32_t> GetDiffData() const;

private:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;

	BlockData<16, 16> m_predicted_data;
	BlockData<16, 16, int32_t> m_diff_data;
};

__codec_end