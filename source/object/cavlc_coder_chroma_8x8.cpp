#include "cavlc_coder_chroma_8x8.h"

#include "cavlc_constant_values.h"

__codec_begin

void CavlcCoderChroma8x8::Code(const BlockData<2, 2, int32_t>& dc_block, const std::vector<BlockData<4, 4, int32_t>> ac_blocks)
{
	CodeDCBlock(dc_block);
	
	m_ac_level_runs.resize(4);
	m_coeff_cost = 0;
	for (uint32_t block_index = 0; block_index < 4; ++block_index)
		CodeACBlock(block_index, ac_blocks[block_index]);

	CheckACCoefficients();

	if (m_ac_cbp == 2)
		m_coded_block_pattern = 2;
}

uint8_t CavlcCoderChroma8x8::GetCodedBlockPattern() const
{
	return m_coded_block_pattern;
}

bool CavlcCoderChroma8x8::HasResetCofficients() const
{
	return m_has_reset_coefficients;
}

LevelAndRuns CavlcCoderChroma8x8::GetDCLevelAndRuns() const
{
    return m_dc_level_runs;
}

std::vector<LevelAndRuns> CavlcCoderChroma8x8::GetACLevelAndRuns() const
{
	return m_ac_level_runs;
}

void CavlcCoderChroma8x8::CodeDCBlock(const BlockData<2, 2, int32_t>& dc_block)
{
	int run = -1;
	for (uint32_t index = 0; index < 4; ++index)
	{
		auto level = dc_block.GetElement(index);
		++run;
		if (level != 0)
		{
			m_dc_level_runs.levels.push_back(level);
			m_dc_level_runs.runs.push_back(run);
			run = -1;
			m_coded_block_pattern = 1;
		}
	}
}

void CavlcCoderChroma8x8::CodeACBlock(uint32_t block_index, const BlockData<4, 4, int32_t>& ac_block)
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
			m_ac_cbp = 2;

			if (level > 1)
				m_coeff_cost += CavlcConstantValues::s_max_coeff_cost;
			else
				m_coeff_cost += CavlcConstantValues::s_coeff_run_cost[run];

			run = -1;
		}
	}
}

void CavlcCoderChroma8x8::CheckACCoefficients()
{
	m_has_reset_coefficients = false;
	if (m_coeff_cost < CavlcConstantValues::s_chroma_coeff_cost_threshold)
	{
		m_has_reset_coefficients = true;
		for (auto& level_runs : m_ac_level_runs)
			level_runs.Clear();
		m_ac_cbp = 0;
	}
}

__codec_end