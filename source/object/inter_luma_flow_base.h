#pragma once

#include <memory>
#include <vector>

#include "block_data.h"
#include "mb_type.h"

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

	virtual void Backend() = 0;

	virtual uint32_t OutputMotionInfo(std::shared_ptr<BytesData> bytes_data) = 0;

	uint32_t OutputCoefficients(std::shared_ptr<BytesData> bytes_data);

	BlockData<16, 16> GetReconstructedData() const;

	int GetDistortion() const;

	uint8_t GetCBP() const;

	uint32_t GetDetailedCBP() const;

protected:
	void CalculateDistortion();

	void TransformAndQuantize();

	void InverseQuantizeAndTransform();

	void CheckCoefficientCost(uint32_t block_8x8);

protected:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	BlockData<16, 16> m_reconstructed_data;
	std::vector<BlockData<4, 4, int32_t>> m_diff_datas;
	BlockData<16, 16, int32_t> m_diff_data;
	std::vector<BlockData<4, 4, int32_t>> m_residual_datas;
	uint32_t m_coefficient_cost{ 0 };
	int m_distortion{ 0 };
	uint8_t m_cbp{ 0 };
	uint32_t m_detailed_cbp{ 0 };

public:
	static std::shared_ptr<InterLumaFlowBase> CreateFlow(MBType mb_type, std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
};

__codec_end