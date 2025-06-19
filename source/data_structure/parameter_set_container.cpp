#include "parameter_set_container.h"

#include "sps.h"
#include "sps_data.h"
#include "pps.h"
#include "pps_data.h"
#include "encoder_config.h"
#include "nalu.h"
#include "ostream.h"

__codec_begin

ParameterSetContainer::ParameterSetContainer()
{
}

ParameterSetContainer::~ParameterSetContainer()
{
}

void ParameterSetContainer::InitConfig(std::shared_ptr<EncoderConfig> config)
{
	m_config = config;
}

void ParameterSetContainer::ConstructSPS()
{
	m_sps = std::make_shared<SPS>();
	auto sps_data = m_sps->GetData();
	sps_data->profile_idc = m_config->profile_idc;
	sps_data->level_idc = m_config->level_idc;
	sps_data->max_num_ref_frames = m_config->ref_frame_number;
	sps_data->pic_width_in_mbs_minus1 = m_config->width / 16 - 1;
	sps_data->pic_height_in_map_units_minus1 = m_config->height / 16 - 1;
}

void ParameterSetContainer::ConstructPPS()
{
	m_pps = std::make_shared<PPS>();
	auto pps_data = m_pps->GetData();
	auto sps_data = m_sps->GetData();
	pps_data->num_ref_idx_l0_default_active_minus1 = sps_data->max_num_ref_frames - 1;
	pps_data->num_ref_idx_l1_default_active_minus1 = sps_data->max_num_ref_frames - 1;
}

void ParameterSetContainer::Serial(std::shared_ptr<OStream> ostream)
{
	SerialSPS(ostream);
	SerialPPS(ostream);
}

void ParameterSetContainer::SerialSPS(std::shared_ptr<OStream> ostream)
{
	Nalu nalu(NaluType::SPS, NaluPriority::HIGHEST);
	nalu.SetData(m_sps->Encapsulate());
	nalu.Serial(ostream);
}

void ParameterSetContainer::SerialPPS(std::shared_ptr<OStream> ostream)
{
	Nalu nalu(NaluType::PPS, NaluPriority::HIGHEST);
	nalu.SetData(m_pps->Encapsulate());
	nalu.Serial(ostream);
}

__codec_end