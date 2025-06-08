#pragma once

#include <numeric>

#include "global_defines.h"

__codec_begin

struct MathUtil
{
	template<typename Ty>
	static Ty Clamp(Ty value, Ty min_value, Ty max_value)
	{
		if (value < min_value)
			return min_value;
		else if (value > max_value)
			return max_value;
		return value;
	}

	template<typename Ty>
	static Ty Average(std::initializer_list<Ty> list)
	{
		Ty sum = std::accumulate(list.begin(), list.end(), 0);
		return std::is_integral_v<Ty> ? sum * 1.0 / list.size() + 0.5 : sum / list.size();
	}

	template<typename Ty>
	static Ty RightShift(Ty value, int bits)
	{
		return (value + (1 << (bits - 1))) >> bits;
	}
};

__codec_end