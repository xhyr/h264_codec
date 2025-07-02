#pragma once

#include <vector>

#include "position.h"

__codec_begin

struct CavlcConstantValues
{
	constexpr static Position s_zigzag_orders[16] = 
	{ 
		{0,0},{1,0},{0,1},{0,2},
		{1,1},{2,0},{3,0},{2,1},
		{1,2},{0,3},{1,3},{2,2},
		{3,1},{3,2},{2,3},{3,3} 
	};


};

__codec_end