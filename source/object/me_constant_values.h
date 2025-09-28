#pragma once

#include <cstdint>
#include <utility>

#include "global_defines.h"

__codec_begin

struct MEConstantValues
{
	constexpr static std::pair<int, int> mv_vertical_limts[21] = {
		{-256,  255},
		{-256,  255},
		{-512,  511},
		{-512,  511},
		{-512,  511},
		{-512,  511},
		{-1024, 1023},
		{-1024, 1023},
		{-1024, 1023},
		{-2048, 2047},
		{-2048, 2047},
		{-2048, 2047},
		{-2048, 2047},
		{-2048, 2047},
		{-2048, 2047},
		{-2048, 2047},
		{-2048, 2047},
		{-32768,32767},
		{-32768,32767},
		{-32768,32767},
		{-32768,32767}
	};

	constexpr static std::pair<int, int> mv_horizontal_limts[2] = {
		{-8192,  8191},
		{-32768, 32767}
	};

};

__codec_end