#pragma once

#include <memory>
#include <vector>

#include "inter_luma_flow_base.h"

__codec_begin

class InterP16x16LumaFlow : public InterLumaFlowBase
{
public:
	InterP16x16LumaFlow();
	~InterP16x16LumaFlow();

	void Frontend() override;

	uint32_t OutputMotionInfo(std::shared_ptr<BytesData> bytes_data) override;

	uint32_t OutputCoefficients(std::shared_ptr<BytesData> bytes_data) override;

private:


};

__codec_end