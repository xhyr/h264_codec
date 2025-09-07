#pragma once

#include "block_data.h"
#include "cavlc_types.h"

__codec_begin

class CavlcPreCoderLuma4x4
{
public:
	void Code(const BlockData<4, 4, int32_t>& block);

	LevelAndRuns GetLevelAndRuns() const;

	uint32_t GetCoefficientCost() const;

	bool IsAllZero() const;

private:
	LevelAndRuns m_level_runs;
	uint32_t m_coefficient_cost{ 0 };
	bool m_all_zero{ true };
};

__codec_end