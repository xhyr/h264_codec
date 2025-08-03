#pragma once

#include <memory>
#include <vector>

#include "block_data.h"
#include "prediction_type.h"

__codec_begin

class Macroblock;
struct EncoderContext;

class IntraLumaFlowBase
{
public:
	IntraLumaFlowBase(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~IntraLumaFlowBase();

	virtual void Frontend() = 0;

	virtual void Backend() = 0;

	BlockData<16, 16> GetReconstructedData() const;

	int GetCost() const;

	uint8_t GetCBP() const;

protected:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	BlockData<16, 16> m_reconstructed_data;

	int m_cost{ 0 };
	uint8_t m_cbp{ 0 };
};

__codec_end