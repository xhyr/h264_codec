#include "mb_intra_header_binaryer.h"

#include "slice.h"
#include "bytes_data.h"
#include "coding_util.h"
#include "cavlc_non_cdc_coder.h"
#include "cavlc_cdc_coder.h"
#include "binary_constant_values.h"

__codec_begin

MBIntraHeaderBinaryer::MBIntraHeaderBinaryer(std::shared_ptr<BytesData> bytes_data) :
	m_bytes_data(bytes_data)
{
}

MBIntraHeaderBinaryer::~MBIntraHeaderBinaryer()
{
}

void MBIntraHeaderBinaryer::OutputMBType(MBType mb_type, uint8_t intra16_offset)
{
	m_mb_type = mb_type;
	if (mb_type == MBType::I16)
		CodingUtil::UE_V(intra16_offset, m_bytes_data);
	else if (mb_type == MBType::I4)
		CodingUtil::UE_V(0, m_bytes_data);
}

void MBIntraHeaderBinaryer::OutputChromaPredMode(IntraChromaPredictionType prediction_type)
{
	CodingUtil::UE_V(static_cast<uint32_t>(prediction_type), m_bytes_data);
}

void MBIntraHeaderBinaryer::OutputCBP(uint8_t cbp)
{
	if (m_mb_type == MBType::I16)
		return;
	
	auto ue = BinaryConstantValues::s_cbp_me_map[cbp][0];
	CodingUtil::UE_V(ue, m_bytes_data);
}

void MBIntraHeaderBinaryer::OutputQPDelta(int qp_delta)
{
	CodingUtil::SE_V(qp_delta, m_bytes_data);
}

__codec_end