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
	int nn = (ue + 1) / 2;
	int i = 0;
	for (; i < 16 && nn != 0; ++i)
		nn /= 2;
	CodingUtil::U_V(2 * i + 1, ue + 1 - pow(2, i), m_bytes_data);
}

void MBBinaryer::OutputQPDelta(int qp_delta)
{
	CodingUtil::SE_V(qp_delta, m_bytes_data);
}

void MBBinaryer::OutputLumaCoeffs(const CavlcDataSource& data_source)
{
	auto slice = m_slice.lock();

	CavlcNonCdcCoder non_cdc_coder(m_mb_addr, slice->GetCavlcContext(), m_bytes_data);
	non_cdc_coder.CodeLumaDC(data_source.luma_dc);
	if (m_cbp & 15)
		non_cdc_coder.CodeLumaACs(data_source.luma_acs);
}

void MBBinaryer::OutputChromaCoeffs(const CavlcDataSource& data_source)
{
	auto slice = m_slice.lock();

	//chroma dc
	if (m_cbp > 15)
	{
		CavlcCdcCoder cdc_coder(m_mb_addr, slice->GetCavlcContext(), m_bytes_data);
		cdc_coder.CodeChromaDC(CavlcDataType::CbDC, data_source.cb_dc);
		cdc_coder.CodeChromaDC(CavlcDataType::CrDC, data_source.cr_dc);
	}

	//chroma ac
	if ((m_cbp >> 4) == 2)
	{
		CavlcNonCdcCoder non_cdc_coder(m_mb_addr, slice->GetCavlcContext(), m_bytes_data);
		non_cdc_coder.CodeChromaACs(CavlcDataType::CbAC, data_source.cb_acs);
		non_cdc_coder.CodeChromaACs(CavlcDataType::CrAC, data_source.cr_acs);
	}
}

__codec_end