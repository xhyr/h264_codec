#pragma once

#include <memory>

#include "global_defines.h"

__codec_begin

struct EncoderConfig;
class OStream;
class SPS;

class ParameterSetContainer
{
public:
	ParameterSetContainer();
	~ParameterSetContainer();

	void InitConfig(std::shared_ptr<EncoderConfig> config);

	void ConstructSPS();

	void SerialSPS(std::shared_ptr<OStream> ostream);

private:
	std::shared_ptr<EncoderConfig> m_config;

	std::shared_ptr<SPS> m_sps;
};

__codec_end