#include "cavlc_pre_coder_luma_4x4.h"

#include "cavlc_constant_values.h"

__codec_begin

void CavlcPreCoderLuma4x4::Code(const BlockData<4, 4, int32_t>& block)
{
	int run = -1;
	for (uint32_t index = 0; index < 16; ++index)
	{
		auto pos = CavlcConstantValues::s_zigzag_orders[index];
		auto level = block.GetElement(pos);
		++run;
		if (level != 0)
		{
			m_level_runs.levels.push_back(level);
			m_level_runs.runs.push_back(run);
			run = -1;
		}
	}
}

LevelAndRuns CavlcPreCoderLuma4x4::GetLevelAndRuns() const
{
	return m_level_runs;
}

__codec_end