#include "encoder.h"

#include "encoder_config.h"
#include "encoder_context.h"
#include "yuv_frame.h"
#include "slice.h"
#include "common_constant_values.h"
#include "log.h"
#include "stream_util.h"

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
	m_context->out_stream = StreamUtil::CreateFileOStream(m_config->output_file_path);
}

bool Encoder::Encode()
{
	ParameterSetContainer parameter_set_container;
	parameter_set_container.InitConfig(m_config);
	parameter_set_container.ConstructSPS();
	parameter_set_container.ConstructPPS();
	parameter_set_container.Serial(m_context->out_stream);

	FILE* out_handle = fopen("output.yuv", "wb");

	while (auto next_yuv_frame = GetNextFrame())
	{
		m_context->yuv_frame = next_yuv_frame;

		auto slice = std::make_shared<Slice>();
		slice->Construct(m_tick, SliceType::I, parameter_set_container.GetActiveSPS(), parameter_set_container.GetActivePPS(), m_context);
		slice->Encode();
		auto cost = slice->GetCost();

		slice->Serial(m_context->out_stream);

		auto frame_data = slice->GetFrameData();

		fwrite(frame_data->y_data.get(), 1, frame_data->width * frame_data->height, out_handle);
		fwrite(frame_data->u_data.get(), 1, frame_data->width * frame_data->height / 4, out_handle);
		fwrite(frame_data->v_data.get(), 1, frame_data->width * frame_data->height / 4, out_handle);

		++m_tick;

		if (m_tick == m_config->frames_to_encode)
			break;
	}

	fclose(out_handle);

	return true;
}

std::shared_ptr<YUVFrame> Encoder::GetNextFrame()
{
	return std::make_shared<YUVFrame>(m_config->width, m_config->height, m_config->input_file_path, m_tick);
}

__codec_end
