#pragma once

#include <memory>
#include <vector>

#include "block_data.h"

__codec_begin

class Macroblock;
struct EncoderContext;
class BytesData;

class InterLumaFlowBase
{
public:
	InterLumaFlowBase(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	virtual ~InterLumaFlowBase();

	virtual void Frontend() = 0;

	virtual uint32_t OutputMotionInfo(std::shared_ptr<BytesData> bytes_data) = 0;

	virtual uint32_t OutputCoefficients(std::shared_ptr<BytesData> bytes_data) = 0;

	BlockData<16, 16> GetReconstructedData() const;

	int GetDistortion() const;

	uint8_t GetCBP() const;

protected:
	void CalculateDistortion();

protected:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	BlockData<16, 16> m_reconstructed_data;

	int m_distortion{ 0 };
	uint8_t m_cbp{ 0 };
};

__codec_end