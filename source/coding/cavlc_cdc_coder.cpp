#include "cavlc_cdc_coder.h"

#include "bytes_data.h"
#include "cavlc_types.h"
#include "cavlc_constant_values.h"
#include "coding_util.h"
#include "cavlc_util.h"
#include "cavlc_context.h"
#include "cavlc_cdc_coder.h"

__codec_begin

CavlcCdcCoder::CavlcCdcCoder(uint32_t mb_addr, std::shared_ptr<CavlcContext> cavlc_context, std::shared_ptr<BytesData> bytes_data) : 
	m_mb_addr(mb_addr),
	m_cavlc_context(cavlc_context),
	m_bytes_data(bytes_data)
{
}

CavlcCdcCoder::~CavlcCdcCoder()
{
}

void CavlcCdcCoder::CodeChromaDC(CavlcDataType data_type, const LevelAndRuns& input)
{
	m_block_index = 0;
	DoCode(input, data_type);
}

void CavlcCdcCoder::DoCode(const LevelAndRuns& input, CavlcDataType data_type)
{
	m_max_coeff_num = CavlcUtil::GetMaxCoeffNum(data_type);
	m_coeff_num = input.GetNonZeroNum();
	m_trailing_ones = input.GetTrailingOnes();
	m_total_zeros = input.GetTotalZeros();
	m_levels = input.levels;
	m_runs = input.runs;

	WriteCoeffNumAndTrailingOnes();

	if (m_coeff_num == 0)
		return;

	m_last_index = m_coeff_num - 1;
	WriteTrailingSigns();
	WriteLevels();
	WriteTotalZeros();
	WriteRuns();
}

void CavlcCdcCoder::WriteCoeffNumAndTrailingOnes()
{
	auto len = CavlcConstantValues::s_cdc_len_table[m_trailing_ones][m_coeff_num];
	uint32_t code = CavlcConstantValues::s_cdc_code_table[m_trailing_ones][m_coeff_num];
	CodingUtil::U_V(len, code, m_bytes_data);
}

void CavlcCdcCoder::WriteTrailingSigns()
{
	if (m_trailing_ones == 0)
		return;
	
	for (int index = m_last_index; index > m_last_index - m_trailing_ones; --index)
		CodingUtil::U_1(m_levels[index] > 0 ? 0 : 1, m_bytes_data);
}

void CavlcCdcCoder::WriteLevels()
{
	bool level_two_or_higher = !(m_coeff_num > 3 && m_trailing_ones == 3);
	int suffix_length = m_coeff_num > 10 && m_trailing_ones < 3 ? 1 : 0;
	for (int index = m_last_index - m_trailing_ones; index >= 0; --index)
	{
		auto level = m_levels[index];
		auto modified_level = level;

		if (level_two_or_higher)
		{
			if (modified_level > 0) --modified_level;
			else ++modified_level;

			level_two_or_higher = false;
		}

		if (suffix_length == 0)
			WriteLevel0(modified_level);
		else
			WriteLevelN(modified_level, suffix_length);

		//update suffix length
		if (abs(level) > CavlcConstantValues::s_suffix_length_inc_table[suffix_length])
			++suffix_length;

		if ((index == m_last_index - m_trailing_ones) && abs(level) > 3)
			suffix_length = 2;
	}
}

void CavlcCdcCoder::WriteTotalZeros()
{
	if (m_coeff_num == m_max_coeff_num)
		return;

	uint8_t len = CavlcConstantValues::s_cdc_total_zeros_len_table[m_coeff_num - 1][m_total_zeros];
	uint8_t code = CavlcConstantValues::s_cdc_total_zeros_code_table[m_coeff_num - 1][m_total_zeros];
	CodingUtil::U_V(len, code, m_bytes_data);
}

void CavlcCdcCoder::WriteRuns()
{
	auto left_zeros = m_total_zeros;
	auto left_coeffs = m_coeff_num;
	for (int index = m_last_index; index >= 0; --index)
	{
		auto run = m_runs[index];

		if (left_zeros == 0 || left_coeffs <= 1)
			break;

		uint8_t table_index = std::min(left_zeros - 1, CavlcConstantValues::s_run_table_no_cdc_size - 1);
		uint8_t len = CavlcConstantValues::s_run_no_cdc_length_table[table_index][run];
		uint8_t code = CavlcConstantValues::s_run_no_cdc_code_table[table_index][run];
		CodingUtil::U_V(len, code, m_bytes_data);

		left_zeros -= run;
		--left_coeffs;
	}
}

void CavlcCdcCoder::WriteLevel0(int level)
{
	uint32_t sign = level < 0 ? 1 : 0;
	uint32_t level_abs = abs(level);

	uint8_t len;
	uint32_t value;
	if (level_abs < 8)
	{
		len = level_abs * 2 + sign - 1;
		value = 1;
	}
	else if (level_abs < 16)
	{
		len = 19;
		value = 16 | ((level_abs << 1) - 16) | sign;
	}
	else
	{
		uint32_t mask = 4096;
		uint32_t prefix_num = 0;
		uint32_t level_abs1 = level_abs + 2032;
		if (level_abs1 >= mask)
		{
			++prefix_num;
			while (level_abs1 >= (mask << prefix_num))
				++prefix_num;
		}
		mask <<= prefix_num;
		value = mask | ((level_abs1 << 1) - mask) | sign;
		len = 28 + (prefix_num << 1);
	}

	CodingUtil::U_V(len, value, m_bytes_data);
}

void CavlcCdcCoder::WriteLevelN(int level, uint32_t suffix_length)
{
	uint32_t sign = level < 0 ? 1 : 0;
	uint32_t level_abs = abs(level) - 1;

	uint8_t len;
	uint32_t value;
	uint32_t shift = suffix_length - 1;
	uint32_t escape = 15 << shift;

	if (level_abs < escape)
	{
		uint32_t sufmask = ~((0xffffffff) << shift);
		uint32_t suffix = level_abs & sufmask;
	
		len = (level_abs >> shift) + 1 + suffix_length;
		value = (2 << shift) | (suffix << 1) | sign;
	}
	else
	{
		uint32_t mask = 4096;
		uint32_t level_abs1 = level_abs - escape + 2048;
		uint32_t prefix_num = 0;

		if (level_abs1 >= 4096)
		{
			++prefix_num;
			while (level_abs1 >= (4096u << prefix_num))
				++prefix_num;
		}

		mask <<= prefix_num;
		value = mask | ((level_abs1 << 1) - mask) | sign;
		len = 28 + (prefix_num << 1);
	}

	CodingUtil::U_V(len, value, m_bytes_data);
}

__codec_end

