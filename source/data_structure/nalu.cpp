#include "nalu.h"

#include "coding_util.h"
#include "file_util.h"

__codec_begin

Nalu::Nalu(NaluType nalu_type) : m_type(nalu_type)
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
	//forbidden_zero_bit

	//nal_ref_idc

	//ebsp
	
}

__codec_end