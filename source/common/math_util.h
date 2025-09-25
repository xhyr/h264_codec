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
	static int CeilLog2(Ty value, int delta = -1)
	{
		value += delta;
		int cnt = 0;
		while (value > 0)
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

	template<typename Ty>
	static Ty Median(Ty a, Ty b, Ty c)
	{
		if (a > b) // a > b
		{
			if (b > c)
				return b; // a > b > c
			else if (a > c)
				return c; // a > c > b
			else
				return a; // c > a > b
		}
		else // b > a
		{
			if (a > c)
				return a; // b > a > c
			else if (b > c)
				return c; // b > c > a
			else
				return b;  // c > b > a
		}
	}

};

__codec_end