#include "cavlc_context.h"

#include "cavlc_util.h"
#include "math_util.h"

__codec_begin

CavlcContext::CavlcContext(uint32_t width_in_mb, uint32_t height_in_mb) : 
	m_width_in_mb(width_in_mb), m_height_in_mb(height_in_mb)
{
	Init();
}

CavlcContext::~CavlcContext()
{
}

void CavlcContext::SetCoeffNum(CavlcDataType data_type, uint32_t mb_addr, uint32_t block_index, uint8_t coeff_num)
{
	if (CavlcUtil::IsDCDataType(data_type))
		return;

	uint32_t total_block_index = GetBlockIndex(data_type, mb_addr, block_index);
	m_coeff_num_map[data_type][total_block_index] = coeff_num;
}

uint8_t CavlcContext::GetNC(CavlcDataType data_type, uint32_t mb_addr, uint32_t block_index) 
{
	data_type = MapDataType(data_type);

	auto [left_available, left_block_index] = GetLeftBlockIndex(data_type, mb_addr, block_index);
	auto [up_available, up_block_index] = GetUpBlockIndex(data_type, mb_addr, block_index);

	uint8_t nc = 0;
	if (left_available && up_available)
		nc = MathUtil::RightShift(m_coeff_num_map[data_type][left_block_index] + m_coeff_num_map[data_type][up_block_index], 1);
	else if (left_available)
		nc = m_coeff_num_map[data_type][left_block_index];
	else if (up_available)
		nc = m_coeff_num_map[data_type][up_block_index];

	return nc;
}

void CavlcContext::Init()
{
	std::vector<CavlcDataType> data_types{ CavlcDataType::NormalLuma, CavlcDataType::LumaAC, CavlcDataType::CbAC, CavlcDataType::CrAC };
	for (auto data_type : data_types)
	{
		uint32_t block_num{ 0 };
		switch (data_type)
		{
		case CavlcDataType::NormalLuma:
		case CavlcDataType::LumaAC:
			block_num = 16 * m_width_in_mb * m_height_in_mb;
			break;
		case CavlcDataType::CbAC:
		case CavlcDataType::CrAC:
			block_num = 4 * m_width_in_mb * m_height_in_mb;
			break;
		default:
			break;
		}
		m_coeff_num_map[data_type].resize(block_num);
	}
}

uint32_t CavlcContext::GetBlockIndex(CavlcDataType data_type, uint32_t mb_addr, uint32_t block_index) const
{
	uint32_t x_in_mb = mb_addr % m_width_in_mb;
	uint32_t y_in_mb = mb_addr / m_width_in_mb;
	uint32_t total_block_index = 0;
	if (CavlcUtil::IsLumaDataType(data_type))
	{
		total_block_index += y_in_mb * m_width_in_mb * 16;
		total_block_index += block_index / 4 * m_width_in_mb * 4;
		total_block_index += x_in_mb * 4 + block_index % 4;
	}
	else
	{
		total_block_index += y_in_mb * m_width_in_mb * 4;
		total_block_index += block_index / 2 * m_width_in_mb * 2;
		total_block_index += x_in_mb * 2 + block_index % 2;
	}
	return total_block_index;
}

std::pair<bool, uint32_t> CavlcContext::GetLeftBlockIndex(CavlcDataType data_type, uint32_t mb_addr, uint32_t block_index) const
{
	uint32_t total_block_index = GetBlockIndex(data_type, mb_addr, block_index);
	if (CavlcUtil::IsLumaDataType(data_type))
	{
		if (total_block_index % (4 * m_width_in_mb) == 0)
			return { false, 0 };
		else
			return { true, total_block_index - 1 };
	}
	else
	{
		if (total_block_index % (2 * m_width_in_mb) == 0)
			return { false, 0 };
		else
			return { true, total_block_index - 1 };
	}
}

std::pair<bool, uint32_t> CavlcContext::GetUpBlockIndex(CavlcDataType data_type, uint32_t mb_addr, uint32_t block_index) const
{
	uint32_t total_block_index = GetBlockIndex(data_type, mb_addr, block_index);
	if (CavlcUtil::IsLumaDataType(data_type))
	{
		if (total_block_index < 4 * m_width_in_mb)
			return { false, 0 };
		else
			return { true, total_block_index - 4 * m_width_in_mb };
	}
	else
	{
		if (total_block_index < 2 * m_width_in_mb)
			return { false, 0 };
		else
			return { true, total_block_index - 2 * m_width_in_mb };
	}
}

CavlcDataType CavlcContext::MapDataType(CavlcDataType data_type) const
{
	if (data_type == CavlcDataType::LumaDC)
		return CavlcDataType::LumaAC;
	return data_type;
}



__codec_end
