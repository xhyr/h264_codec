#include "cavlc_non_cdc_coder.h"

#include "bytes_data.h"
#include "cavlc_types.h"
#include "cavlc_constant_values.h"
#include "coding_util.h"

__codec_begin

CavlcNonCdcCoder::CavlcNonCdcCoder() : m_bytes_data(std::make_shared<BytesData>())
{
}

CavlcNonCdcCoder::~CavlcNonCdcCoder()
{
}

void CavlcNonCdcCoder::Code(const LevelAndRuns& input)
{
	m_coeff_num = input.GetNonZeroNum();
	m_trailing_ones = input.GetTrailingOnes();
	m_total_zeros = input.GetTotalZeros();

	ObtainVlcTableIndex();

	WriteCoeffNumAndTrailingOnes();

	if (m_coeff_num == 0)
		return;

	WriteTrailingSigns();



}

void CavlcNonCdcCoder::ObtainVlcTableIndex()
{
	m_vlc_table_index = 0;
}

void CavlcNonCdcCoder::WriteCoeffNumAndTrailingOnes()
{
	if (m_vlc_table_index == 3)
	{
		if (m_trailing_ones > 0)
		{
			CodingUtil::U_V(4, m_coeff_num, m_bytes_data);
			CodingUtil::U_V(2, m_trailing_ones - 1, m_bytes_data);
		}
		else CodingUtil::U_V(6, 3, m_bytes_data);
	}
	else
	{
		auto len = CavlcConstantValues::s_non_cdc_len_table[m_vlc_table_index][m_trailing_ones][m_coeff_num];
		uint32_t code = CavlcConstantValues::s_non_cdc_code_table[m_vlc_table_index][m_trailing_ones][m_coeff_num];
		CodingUtil::U_V(len, code, m_bytes_data);
	}
}

void CavlcNonCdcCoder::WriteTrailingSigns()
{

}

__codec_end