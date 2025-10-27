#include "mb_inter_header_binaryer.h"

#include "coding_util.h"
#include "binary_constant_values.h"

__codec_begin

MBInterHeaderBinaryer::MBInterHeaderBinaryer(std::shared_ptr<BytesData> bytes_data) : 
	m_bytes_data(bytes_data)
{
}

MBInterHeaderBinaryer::~MBInterHeaderBinaryer()
{
}

void MBInterHeaderBinaryer::OutputMBSkipRun(uint32_t skip_run)
{
	CodingUtil::UE_V(0, m_bytes_data);
}

void MBInterHeaderBinaryer::OutputMBType(MBType mb_type)
{
	switch (mb_type)
	{
	case MBType::P16x16:
		CodingUtil::UE_V(0, m_bytes_data);
		break;
	case MBType::P16x8:
		CodingUtil::UE_V(1, m_bytes_data);
		break;
	case MBType::P8x16:
		CodingUtil::UE_V(2, m_bytes_data);
		break;
	case MBType::P8x8:
		CodingUtil::UE_V(4, m_bytes_data); //zero ref
		break;
	}
}

void MBInterHeaderBinaryer::OutputCBP(uint8_t cbp)
{
	auto ue = BinaryConstantValues::s_cbp_me_map[cbp][1];
	CodingUtil::UE_V(ue, m_bytes_data);
}

void MBInterHeaderBinaryer::OutputQPDelta(int qp_delta)
{
	CodingUtil::SE_V(qp_delta, m_bytes_data);
}

__codec_end