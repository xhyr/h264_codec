#pragma once

#include <memory>
#include <vector>

#include "cavlc_types.h"

__codec_begin

class BytesData;
class CavlcContext;
struct LevelAndRuns;

class CavlcNonCdcCoder
{
public:
	CavlcNonCdcCoder(uint32_t mb_addr, std::shared_ptr<CavlcContext> cavlc_context, std::shared_ptr<BytesData> bytes_data);
	~CavlcNonCdcCoder();

	void CodeLumaDC(const LevelAndRuns& input);

	void CodeLumaACs(const std::vector<LevelAndRuns>& inputs);

private:
	void DoCode(const LevelAndRuns& input, CavlcDataType data_type);

	void ObtainVlcTableIndex(CavlcDataType data_type);

	void WriteCoeffNumAndTrailingOnes();

	void WriteTrailingSigns();

	void WriteLevels();

	void WriteTotalZeros();

	void WriteRuns();

	void WriteLevel0(int level);

	void WriteLevelN(int level, uint32_t suffix_length);

private:
	uint32_t m_mb_addr;
	std::shared_ptr<CavlcContext> m_cavlc_context;
	std::shared_ptr<BytesData> m_bytes_data;

	uint8_t m_block_index;
	uint8_t m_coeff_num;
	uint8_t m_max_coeff_num;
	uint8_t m_trailing_ones;
	uint8_t m_total_zeros;
	uint8_t m_vlc_table_index;
	uint8_t m_last_index;

	std::vector<int32_t> m_levels;
	std::vector<uint8_t> m_runs;

};

__codec_end