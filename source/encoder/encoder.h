#pragma once

#include <memory>

#include "parameter_set_container.h"

__codec_begin

struct EncoderConfig;
struct EncoderContext;
struct YUVFrame;

class Encoder
{
public:
	explicit Encoder(std::shared_ptr<EncoderConfig> config);

	void PrepareContext();

	bool Encode();

private:
	std::shared_ptr<YUVFrame> GetNextFrame();

private:
	std::shared_ptr<EncoderConfig> m_config;
	std::shared_ptr<EncoderContext> m_context;
	
	ParameterSetContainer m_parameter_set_container;

	uint32_t m_tick{ 0 };
};

__codec_end