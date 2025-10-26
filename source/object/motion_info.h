#pragma once

#include <cstdint>

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

struct SearchInfo
{
	uint32_t x_in_block;
	uint32_t y_in_block;
	uint32_t width_in_block;
	uint32_t height_in_block;
	uint8_t ref_id;
};

__codec_end