#include "motion_info.h"

__codec_begin

MotionVector operator+(const MotionVector& left, const MotionVector& right)
{
	return MotionVector{ left.x + right.x, left.y + right.y };
}

MotionVector operator-(const MotionVector& left, const MotionVector& right)
{
	return MotionVector{ left.x - right.x, left.y - right.y };
}

__codec_end