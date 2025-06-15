#pragma once

#include <memory>

#include "global_defines.h"

__codec_begin

struct EncoderConfig;
struct EncoderContext;

class Encoder
{
public:
	explicit Encoder(std::shared_ptr<EncoderConfig> config);

	void PrepareContext();



private:
	std::shared_ptr<EncoderConfig> m_config;
	std::shared_ptr<EncoderContext> m_context;
};

__codec_end