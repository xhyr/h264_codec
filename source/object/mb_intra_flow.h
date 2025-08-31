#pragma once

#include <memory>

#include "mb_flow_base.h"

__codec_begin

class MBIntraRDOptimizer;

class MBIntraFlow : public MBFlowBase
{
public:
	MBIntraFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~MBIntraFlow();

	void DoEncode() override;

	void Binary(std::shared_ptr<BytesData> bytes_data) override;

private:
	std::unique_ptr<MBIntraRDOptimizer> m_rd_optimizer;

};

__codec_end