#include "color_converter.h"

#include "math_util.h"

__codec_begin

std::tuple<uint8_t, uint8_t, uint8_t> ColorConverter::RGB2YUV709Full(int r, int g, int b)
{
	int y = 0.2126 * r + 0.7152 * g + 0.0722 * b + 0.5;
	y = MathUtil::Clamp(y, 0, 255);

	int u = -0.1146 * r - 0.3854 * g + 0.5 * b + 128 +  0.5;
	u = MathUtil::Clamp(u, 0, 255);

	int v = 0.5 * r - 0.4542 * g - 0.0458 * b + 128 + 0.5;
	v = MathUtil::Clamp(v, 0, 255);

	return std::make_tuple<uint8_t>(y, u, v);
}

__codec_end
