#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "global_defines.h"

__codec_begin

class Macroblock;

struct FilterUtil
{
	static std::vector<uint8_t> GetVerticalStrengths(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q, uint8_t edge_index);

	static std::vector<uint8_t> GetHorizontalStrengths(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q, uint8_t edge_index);

	static bool IsLumaAllZeroVerticalEdge(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q, uint8_t edge_index, uint8_t i);

	static bool IsLumaAllZeroHorizontalEdge(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q, uint8_t edge_index, uint8_t i);

	static bool IsMotionDifferentVerticalEdge(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q, uint8_t edge_index, uint8_t i);

	static bool IsMotionDifferentHorizontalEdge(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q, uint8_t edge_index, uint8_t i);
};

__codec_end