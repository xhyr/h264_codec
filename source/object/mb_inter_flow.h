#pragma once

#include <memory>

#include "mb_flow_base.h"

__codec_begin

class MBInterRDOptimizer;

class MBInterFlow : public MBFlowBase
{
public:
	MBInterFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~MBInterFlow();

	void DoEncode() override;

	void Binary(std::shared_ptr<BytesData> bytes_data) override;

private:
	std::unique_ptr<MBInterRDOptimizer> m_rd_optimizer;
};

__codec_end