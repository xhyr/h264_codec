#pragma once

#include "global_defines.h"

__codec_begin

struct MotionVector
{
	int mv[2]{ 0 };
};

struct MotionInfo
{
	int ref_id{ -1 };
	MotionVector mv;
};

__codec_end