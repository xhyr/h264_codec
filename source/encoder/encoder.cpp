#include "encoder.h"

#include "encoder_config.h"
#include "encoder_context.h"
#include "yuv_frame.h"

__codec_begin

Encoder::Encoder(std::shared_ptr<EncoderConfig> config) : m_config(config)
{
}

void Encoder::PrepareContext()
{
	m_context = std::make_shared<EncoderContext>();
	m_context->config = m_config;
	m_context->width = m_config->width;
	m_context->height = m_config->height;
	m_context->yuv_frame = std::make_shared<YUVFrame>(m_config->width, m_config->height, m_config->input_file_path);
}

__codec_end
