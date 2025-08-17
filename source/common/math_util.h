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

	template<typename Ty>
	static int CeilLog2(Ty value)
	{
		--value;
		int cnt = 0;
		while (value != 0)
		{
			value >>= 1;
			++cnt;
		}
		return cnt;
	}

	template<typename Ty>
	static Ty Square(Ty value)
	{
		return value * value;
	}

};

__codec_end