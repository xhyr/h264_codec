#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "global_defines.h"

__codec_begin

class Macroblock;

struct FilterUtil
{
	static std::vector<uint8_t> GetVerticalStrengths(std::shared_ptr<Macroblock> mb, uint8_t edge_index);

	static std::vector<uint8_t> GetHorizontalStrengths(std::shared_ptr<Macroblock> mb, uint8_t edge_index);
};

__codec_end