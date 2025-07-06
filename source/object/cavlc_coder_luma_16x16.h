#pragma once

#include "block_data.h"
#include "cavlc_types.h"

__codec_begin

class CavlcCoderLuma16x16
{
public:
	void Code(const BlockData<4, 4, int32_t>& dc_block, const std::vector<BlockData<4, 4, int32_t>> ac_blocks);

	uint8_t GetCodedBlockPattern() const;

private:
	void CodeDCBlock(const BlockData<4, 4, int32_t>& dc_block);

	void CodeACBlock(uint32_t block_index, const BlockData<4, 4, int32_t>& ac_block);

private:
	uint8_t m_coded_block_pattern{ 0 };
	LevelAndRuns m_dc_level_runs;
	std::vector<LevelAndRuns> m_ac_level_runs;
};

__codec_end