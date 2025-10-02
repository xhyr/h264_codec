#pragma once

#include <memory>
#include <vector>

#include "block_data.h"
#include "prediction_type.h"

__codec_begin

class Macroblock;
struct EncoderContext;
class BytesData;

class IntraLumaFlowBase
{
public:
	IntraLumaFlowBase(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	virtual ~IntraLumaFlowBase();

	virtual void Frontend() = 0;

	virtual void OutputPredictionTypes(std::shared_ptr<BytesData> bytes_data);

	virtual uint32_t OutputCoefficients(std::shared_ptr<BytesData> bytes_data) = 0;

	BlockData<16, 16> GetReconstructedData() const;

	int GetDistortion() const;

	uint8_t GetCBP() const;

	uint32_t GetDetailedCBP() const;

protected:
	void CalculateDistortion();

protected:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	BlockData<16, 16> m_reconstructed_data;

	int m_distortion{ 0 };
	uint8_t m_cbp{ 0 };
	uint32_t m_detailed_cbp{ 0 };
};

__codec_end