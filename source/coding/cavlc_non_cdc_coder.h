#pragma once

#include <memory>

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

private:
	std::shared_ptr<BytesData> m_bytes_data;

	uint8_t m_coeff_num;
	uint8_t m_trailing_ones;
	uint8_t m_total_zeros;
	uint8_t m_vlc_table_index;

};

__codec_end