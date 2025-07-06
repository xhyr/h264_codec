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

	constexpr static int s_max_coeff_cost{ 999999 };
	constexpr static int s_coeff_run_cost[] = { 3,2,2,1,1,1,0,0,0,0,0,0,0,0,0,0 };
	constexpr static int s_chroma_coeff_cost_threshold{ 4 };
};

__codec_end