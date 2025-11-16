#include "cavlc_context.h"

#include <algorithm>

#include "cavlc_util.h"
#include "math_util.h"
#include "segment_util.h"

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
	m_coeff_num_map[MapDataType(data_type)][total_block_index] = coeff_num;
}

uint8_t CavlcContext::GetNC(CavlcDataType data_type, uint32_t mb_addr, uint32_t block_index) 
{
	auto context_type = MapDataType(data_type);

	auto [left_available, left_block_index] = GetLeftBlockIndex(data_type, mb_addr, block_index);
	auto [up_available, up_block_index] = GetUpBlockIndex(data_type, mb_addr, block_index);

	uint8_t nc = 0;
	if (left_available && up_available)
		nc = MathUtil::RightShift(m_coeff_num_map[context_type][left_block_index] + m_coeff_num_map[context_type][up_block_index], 1);
	else if (left_available)
		nc = m_coeff_num_map[context_type][left_block_index];
	else if (up_available)
		nc = m_coeff_num_map[context_type][up_block_index];

	return nc;
}

std::vector<uint8_t> CavlcContext::GetMBLumaCoeffNums(uint32_t mb_addr) 
{
	std::vector<uint8_t> coeff_nums(16);
	for (uint8_t i = 0; i < 16; ++i)
	{
		auto index = GetBlockIndex(CavlcDataType::NormalLuma, mb_addr, i);
		coeff_nums[i] = m_coeff_num_map[CavlcContextType::Luma][index];
	}

	return coeff_nums;
}

void CavlcContext::SetMBLumaCoeffNums(uint32_t mb_addr, const std::vector<uint8_t>& coeff_nums)
{
	for (uint8_t i = 0; i < 16; ++i)
	{
		auto index = GetBlockIndex(CavlcDataType::NormalLuma, mb_addr, i);
		m_coeff_num_map[CavlcContextType::Luma][index] = coeff_nums[i];
	}
}

void CavlcContext::ResetBlockCoeffNums(uint32_t mb_addr, uint8_t x_in_block, uint8_t y_in_block, CavlcDataType data_type)
{

}

void CavlcContext::ResetLumaBlockCoeffNums(uint32_t mb_addr, uint8_t block_8x8, CavlcDataType data_type)
{
	auto [start_x, start_y] = SegmentUtil::GetLumaDataRect<8, 8>(block_8x8);
	for (uint32_t y_in_block = start_y / 4; y_in_block < start_y / 4 + 2; ++y_in_block)
	{
		for (uint32_t x_in_block = start_x / 4; x_in_block < start_x / 4 + 2; ++x_in_block)
		{
			SetCoeffNum(data_type, mb_addr, y_in_block * 4 + x_in_block, 0);
		}
	}
}

void CavlcContext::Reset()
{
	for (auto& item : m_coeff_num_map)
		std::fill(item.second.begin(), item.second.end(), 0);
}

void CavlcContext::Init()
{
	std::vector<CavlcContextType> context_types{ CavlcContextType::Luma, CavlcContextType::Cb, CavlcContextType::Cr };
	for (auto context_type : context_types)
	{
		uint32_t block_num{ 0 };
		switch (context_type)
		{
		case CavlcContextType::Luma:
			block_num = 16 * m_width_in_mb * m_height_in_mb;
			break;
		case CavlcContextType::Cb:
		case CavlcContextType::Cr:
			block_num = 4 * m_width_in_mb * m_height_in_mb;
			break;
		default:
			break;
		}
		m_coeff_num_map[context_type].resize(block_num);
	}
}

uint32_t CavlcContext::GetBlockIndex(CavlcDataType data_type, uint32_t mb_addr, uint32_t block_index) const
{
	uint32_t x_in_mb = mb_addr % m_width_in_mb;
	uint32_t y_in_mb = mb_addr / m_width_in_mb;
	uint32_t total_block_index = 0;
	if (CavlcUtil::IsLumaDataType(data_type))
	{
		uint32_t x_in_block = x_in_mb * 4 + block_index % 4;
		uint32_t y_in_block = y_in_mb * 4 + block_index / 4;
		total_block_index = y_in_block * m_width_in_mb * 4 + x_in_block;
	}
	else
	{
		uint32_t x_in_block = x_in_mb * 2 + block_index % 2;
		uint32_t y_in_block = y_in_mb * 2 + block_index / 2;
		total_block_index = y_in_block * m_width_in_mb * 2 + x_in_block;
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

CavlcContextType CavlcContext::MapDataType(CavlcDataType data_type) const
{
	if (data_type == CavlcDataType::LumaDC || data_type == CavlcDataType::LumaAC || data_type == CavlcDataType::NormalLuma)
		return CavlcContextType::Luma;
	else if (data_type == CavlcDataType::CbAC)
		return CavlcContextType::Cb;
	else if (data_type == CavlcDataType::CrAC)
		return CavlcContextType::Cr;
}



__codec_end
