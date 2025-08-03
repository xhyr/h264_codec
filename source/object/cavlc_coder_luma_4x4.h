#pragma once

#include "block_data.h"
#include "cavlc_types.h"

__codec_begin

class CavlcCoderLuma4x4
{
public:
	void Code(const BlockData<4, 4, int32_t>& block);

	LevelAndRuns GetLevelAndRuns() const;

private:
	LevelAndRuns m_level_runs;
};

__codec_end