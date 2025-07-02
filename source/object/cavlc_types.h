#pragma once

#include <cstdint>
#include <vector>

#include "global_defines.h"

__codec_begin

struct LevelAndRuns
{
	std::vector<int32_t> levels;
	std::vector<uint8_t> runs;
};

__codec_end