#pragma once

#include <memory>

#include "global_defines.h"

__codec_begin

struct EncoderConfig;
class OStream;
class SPS;
class PPS;

class ParameterSetContainer
{
public:
	ParameterSetContainer();
	~ParameterSetContainer();

	void InitConfig(std::shared_ptr<EncoderConfig> config);

	void ConstructSPS();

	void ConstructPPS();

	void Serial(std::shared_ptr<OStream> ostream);

	std::shared_ptr<SPS> GetActiveSPS();

	std::shared_ptr<PPS> GetActivePPS();

private:
	void SerialSPS(std::shared_ptr<OStream> ostream);

	void SerialPPS(std::shared_ptr<OStream> ostream);

private:
	std::shared_ptr<EncoderConfig> m_config;

	std::shared_ptr<SPS> m_sps;
	std::shared_ptr<PPS> m_pps;
};

__codec_end