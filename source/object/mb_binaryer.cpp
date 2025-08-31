#include "mb_binaryer.h"

#include "slice.h"
#include "bytes_data.h"
#include "coding_util.h"
#include "cavlc_non_cdc_coder.h"
#include "cavlc_cdc_coder.h"
#include "binary_constant_values.h"

__codec_begin

MBBinaryer::MBBinaryer(std::weak_ptr<Slice> slice, uint32_t mb_addr, std::shared_ptr<BytesData> bytes_data) :
	m_slice(slice), m_mb_addr(mb_addr), m_bytes_data(bytes_data)
{
}

MBBinaryer::~MBBinaryer()
{
}

void MBBinaryer::OutputMBType(MBType mb_type, uint8_t intra16_offset)
{
	auto slice = m_slice.lock();
	m_slice_type = slice->GetType();
	m_mb_type = mb_type;
	if (m_slice_type == SliceType::I)
	{
		if (mb_type == MBType::I16)
			CodingUtil::UE_V(intra16_offset, m_bytes_data);
		else if (mb_type == MBType::I4)
			CodingUtil::UE_V(0, m_bytes_data);
	}
	else if (m_slice_type == SliceType::P)
	{
		switch (mb_type)
		{
		case codec::MBType::P16x16:
			CodingUtil::UE_V(0, m_bytes_data);
			break;
		case codec::MBType::P16x8:
			CodingUtil::UE_V(1, m_bytes_data);
			break;
		case codec::MBType::P8x16:
			CodingUtil::UE_V(2, m_bytes_data);
			break;
		case codec::MBType::I4:
			CodingUtil::UE_V(6, m_bytes_data);
			break;
		case codec::MBType::I16:
			CodingUtil::UE_V(6 + intra16_offset, m_bytes_data);
			break;
		default:
			break;
		}
	}

}

void MBBinaryer::OutputChromaPredMode(IntraChromaPredictionType prediction_type)
{
	CodingUtil::UE_V(static_cast<uint32_t>(prediction_type), m_bytes_data);
}

void MBBinaryer::OutputCBP(uint8_t cbp)
{
	m_cbp = cbp;
	if (m_mb_type == MBType::I16)
		return;
	
	auto ue = BinaryConstantValues::s_cbp_me_map[cbp][0];
	CodingUtil::UE_V(ue, m_bytes_data);
}

void MBBinaryer::OutputQPDelta(int qp_delta)
{
	CodingUtil::SE_V(qp_delta, m_bytes_data);
}

__codec_end