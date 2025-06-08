#pragma once

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
};

__codec_end