#pragma once

#include <cstdint>

#include "block_data.h"

__codec_begin

struct Transformer
{
	static BlockData<4, 4, int> Hadamard(BlockData<4, 4, int> block_data);


};

__codec_end