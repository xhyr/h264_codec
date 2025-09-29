#pragma once

#include "global_defines.h"

__codec_begin

struct MotionVector
{
	int x{ 0 };
	int y{ 0 };
};

MotionVector operator+(const MotionVector& left, const MotionVector& right);

MotionVector operator-(const MotionVector& left, const MotionVector& right);

struct MotionInfo
{
	int ref_id{ -1 };
	MotionVector mv;
};

__codec_end