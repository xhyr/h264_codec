#include "pps.h"

#include "pps_data.h"

__codec_begin

PPS::PPS() : m_data(std::make_shared<PPSData>())
{

}

PPS::~PPS()
{
}

std::shared_ptr<PPSData> PPS::GetData()
{
	return m_data;
}

std::shared_ptr<BytesData> PPS::Encapsulate()
{
	return m_data->Convert2BytesData();
}

__codec_end