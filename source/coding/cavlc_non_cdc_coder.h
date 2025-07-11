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
	explicit CavlcNonCdcCoder(std::shared_ptr<BytesData> bytes_data);
	~CavlcNonCdcCoder();

	void CodeDC(const LevelAndRuns& input);

	void CodeACs(const std::vector<LevelAndRuns>& inputs);

private:
	void DoCode(const LevelAndRuns& input, uint8_t max_coeff_num);

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
	uint8_t m_max_coeff_num;
	uint8_t m_trailing_ones;
	uint8_t m_total_zeros;
	uint8_t m_vlc_table_index;
	uint8_t m_last_index;

	std::vector<int32_t> m_levels;
	std::vector<uint8_t> m_runs;

};

__codec_end