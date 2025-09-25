#pragma once

#include <memory>
#include <vector>

#include "inter_luma_flow_base.h"

__codec_begin

class InterP16x16LumaPredictor;

class InterP16x16LumaFlow : public InterLumaFlowBase
{
public:
	InterP16x16LumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~InterP16x16LumaFlow();

	void Frontend() override;

	void Backend() override;

	uint32_t OutputMotionInfo(std::shared_ptr<BytesData> bytes_data) override;

	uint32_t OutputCoefficients(std::shared_ptr<BytesData> bytes_data) override;

private:
	void Predict();

	void TransformAndQuantize();

	void InverseQuantizeAndTransform();

	void Reconstruct();

	void CheckCoefficientCost(uint32_t block_8x8);

private:
	std::unique_ptr<InterP16x16LumaPredictor> m_predictor;
	std::vector<BlockData<4, 4, int32_t>> m_diff_datas;
	BlockData<16, 16, int32_t> m_diff_data;
	std::vector<BlockData<4, 4, int32_t>> m_residual_datas;
	uint32_t m_coefficient_cost{ 0 };
};

__codec_end