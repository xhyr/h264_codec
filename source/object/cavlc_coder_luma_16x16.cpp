#include "cavlc_coder_luma_16x16.h"

#include "cavlc_constant_values.h"

__codec_begin

void CavlcCoderLuma16x16::Code(const BlockData<4, 4, int32_t>& dc_block, const std::vector<BlockData<4, 4, int32_t>> ac_blocks)
{
	CodeDCBlock(dc_block);
	
	m_ac_level_runs.resize(16);
	for (uint32_t block_index = 0; block_index < 16; ++block_index)
		CodeACBlock(block_index, ac_blocks[block_index]);
}

uint8_t CavlcCoderLuma16x16::GetCodedBlockPattern() const
{
	return m_coded_block_pattern;
}

LevelAndRuns CavlcCoderLuma16x16::GetDCLevelAndRuns() const
{
	return m_dc_level_runs;
}

std::vector<LevelAndRuns> CavlcCoderLuma16x16::GetACLevelAndRuns() const
{
	return m_ac_level_runs;
}

void CavlcCoderLuma16x16::CodeDCBlock(const BlockData<4, 4, int32_t>& dc_block)
{
	int run = -1;
	for (uint32_t index = 0; index < 16; ++index)
	{
		auto pos = CavlcConstantValues::s_zigzag_orders[index];
		auto level = dc_block.GetElement(pos);
		++run;
		if (level != 0)
		{
			m_dc_level_runs.levels.push_back(level);
			m_dc_level_runs.runs.push_back(run);
			run = -1;
		}
	}
}

void CavlcCoderLuma16x16::CodeACBlock(uint32_t block_index, const BlockData<4, 4, int32_t>& ac_block)
{
	int run = -1;
	auto& level_runs = m_ac_level_runs[block_index];
	for (uint32_t index = 1; index < 16; ++index)
	{
		auto pos = CavlcConstantValues::s_zigzag_orders[index];
		auto level = ac_block.GetElement(pos);
		++run;
		if (level != 0)
		{
			level_runs.levels.push_back(level);
			level_runs.runs.push_back(run);
			m_coded_block_pattern = 15;
			run = -1;
		}
	}
}

__codec_end