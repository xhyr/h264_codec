#pragma once

#include <cstdint>
#include <vector>

#include "global_defines.h"

__codec_begin

struct LevelAndRuns
{
	std::vector<int32_t> levels;
	std::vector<uint8_t> runs;

	void Clear();

	int GetNonZeroNum() const;

	int GetTrailingOnes() const;

	int GetTotalZeros() const;
};

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

__codec_end