#include "nalu.h"

#include "coding_util.h"
#include "ostream.h"
#include "bytes_data.h"
#include "coding_util.h"
#include "common_constant_values.h"

__codec_begin

Nalu::Nalu(NaluType nalu_type, NaluPriority nalu_priority) : m_type(nalu_type), m_priority(nalu_priority)
{
}

Nalu::~Nalu()
{
}

void Nalu::SetData(std::shared_ptr<BytesData> bytes_data)
{
	m_data = CodingUtil::SODB2EBSP(bytes_data);
}

void Nalu::Serial(std::shared_ptr<OStream> ostream)
{
	std::shared_ptr<BytesData> bytes_data = std::make_shared<BytesData>();
	
	//start code
	for (uint8_t i = 0; i < CommonConstantValues::s_zero_bytes_start_code; ++i)
		bytes_data->PushByte(0);
	bytes_data->PushByte(1);

	//forbidden_zero_bit
	CodingUtil::U_1(0, bytes_data);
	//nal_ref_idc
	CodingUtil::U_V(2, static_cast<uint8_t>(m_priority), bytes_data);
	//nal_unit_type
	CodingUtil::U_V(5, static_cast<uint8_t>(m_type), bytes_data);

	ostream->PushBytesData(bytes_data);

	//ebsp
	ostream->PushBytesData(m_data);
}

__codec_end