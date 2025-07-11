#pragma once

#include <memory>
#include <vector>

#include "global_defines.h"

__codec_begin

struct LevelAndRuns;
class BytesData;

class CavlcNonCdcCoder
{
public:
	CavlcNonCdcCoder();
	~CavlcNonCdcCoder();

	void Code(const LevelAndRuns& input);

private:
	void ObtainVlcTableIndex();

	void WriteCoeffNumAndTrailingOnes();

	void WriteTrailingSigns();

	void WriteLevels();

	void WriteTotalZeros();

	void WriteRuns();

	void WriteLevel0(int level);

	void WriteLevelN(int level, uint32_t suffix_length);

private:
	std::shared_ptr<BytesData> m_bytes_data;

	uint8_t m_coeff_num;
	uint8_t m_trailing_ones;
	uint8_t m_total_zeros;
	uint8_t m_vlc_table_index;
	uint8_t m_last_index;

	std::vector<int32_t> m_levels;
	std::vector<uint8_t> m_runs;

};

__codec_end