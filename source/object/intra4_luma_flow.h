#pragma once

#include <memory>
#include <vector>

#include "intra_luma_flow_base.h"
#include "prediction_type.h"

__codec_begin

class Macroblock;
struct EncoderContext;
class Intra4LumaFlowNode;
class BytesData;

class Intra4LumaFlow : public IntraLumaFlowBase
{
public:
	Intra4LumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~Intra4LumaFlow();

	void Frontend() override;

	void OutputPredictionTypes(std::shared_ptr<BytesData> bytes_data) override;

	uint32_t OutputCoefficients(std::shared_ptr<BytesData> bytes_data) override;

	std::vector<Intra4LumaPredictionType> GetPredictionTypes() const;

private:
	std::vector<Intra4LumaPredictionType> m_prediction_types;
	std::vector<std::shared_ptr<Intra4LumaFlowNode>> m_nodes;
};

__codec_end