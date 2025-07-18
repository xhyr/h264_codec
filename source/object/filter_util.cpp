#include "filter_util.h"

#include "macroblock.h"

__codec_begin

std::vector<uint8_t> FilterUtil::GetVerticalStrengths(std::shared_ptr<Macroblock> mb, uint8_t edge_index)
{
	uint8_t strength = edge_index == 0 ? 4 : 3;
	return std::vector<uint8_t>(4, strength);
}

std::vector<uint8_t> FilterUtil::GetHorizontalStrengths(std::shared_ptr<Macroblock> mb, uint8_t edge_index)
{
	uint8_t strength = edge_index == 0 ? 4 : 3;
	return std::vector<uint8_t>(4, strength);
}

__codec_end

