#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "cavlc_types.h"

__codec_begin

class CavlcContext
{
public:
	CavlcContext(uint32_t width_in_mb, uint32_t height_in_mb);
	~CavlcContext();

	void SetCoeffNum(CavlcDataType data_type, uint32_t mb_addr, uint32_t block_index, uint8_t coeff_num);

	uint8_t GetNC(CavlcDataType data_type, uint32_t mb_addr, uint32_t block_index);

private:
	void Init();

	uint32_t GetBlockIndex(CavlcDataType data_type, uint32_t mb_addr, uint32_t block_index) const;

	std::pair<bool, uint32_t> GetLeftBlockIndex(CavlcDataType data_type, uint32_t mb_addr, uint32_t block_index) const;

	std::pair<bool, uint32_t> GetUpBlockIndex(CavlcDataType data_type, uint32_t mb_addr, uint32_t block_index) const;

	CavlcContextType MapDataType(CavlcDataType data_type) const;

private:
	uint32_t m_width_in_mb;
	uint32_t m_height_in_mb;

	std::unordered_map<CavlcContextType, std::vector<uint8_t>> m_coeff_num_map;
};

__codec_end