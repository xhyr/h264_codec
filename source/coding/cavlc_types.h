#pragma once

#include <cstdint>
#include <vector>

#include "global_defines.h"

__codec_begin

enum class CavlcDataType
{
	NormalLuma = 0,
	LumaDC,
	LumaAC,
	CbDC,
	CrDC,
	CbAC,
	CrAC
};

struct LevelAndRuns
{
	std::vector<int32_t> levels;
	std::vector<uint8_t> runs;

	void Clear();

	int GetNonZeroNum() const;

	int GetTrailingOnes() const;

	int GetTotalZeros() const;
};

struct CavlcDataSource
{
	LevelAndRuns luma_dc;
	std::vector<LevelAndRuns> luma_acs;

	LevelAndRuns cb_dc;
	std::vector<LevelAndRuns> cb_acs;

	LevelAndRuns cr_dc;
	std::vector<LevelAndRuns> cr_acs;
};

__codec_end