#include "sps.h"

#include "sps_data.h"

__codec_begin

SPS::SPS() : m_data(std::make_shared<SPSData>())
{
}

SPS::~SPS()
{
}

std::shared_ptr<SPSData> SPS::GetData()
{
	return m_data;
}

std::shared_ptr<BytesData> SPS::Encapsulate()
{
	return m_data->Convert2BytesData();
}

__codec_end
