#include "encoder.h"

#include "encoder_config.h"
#include "encoder_context.h"
#include "yuv_frame.h"
#include "slice.h"
#include "common_constant_values.h"
#include "log.h"

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
	m_context->width_in_mb = m_config->width / CommonConstantValues::s_mb_width;
	m_context->height_in_mb = m_config->height / CommonConstantValues::s_mb_height;
	m_context->mb_num = m_context->width_in_mb * m_context->height_in_mb;
	m_context->yuv_frame = std::make_shared<YUVFrame>(m_config->width, m_config->height, m_config->input_file_path);
}

bool Encoder::Encode()
{
	ParameterSetContainer parameter_set_container;
	parameter_set_container.InitConfig(m_config);
	parameter_set_container.ConstructSPS();
	parameter_set_container.ConstructPPS();

	auto slice = std::make_shared<Slice>();
	slice->Construct(SliceType::I, parameter_set_container.GetActiveSPS(), parameter_set_container.GetActivePPS());
	slice->Encode(m_context);

	auto cost = slice->GetCost();
	
	LOGINFO("cost = %d, average_cost = %lf.", cost, cost * 1.0 / m_context->width / m_context->height);

	return true;
}

__codec_end
