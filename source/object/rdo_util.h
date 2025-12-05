#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

struct RDOUtil
{
	static double GetLambdaMode(int qp);

	static double GetLambdaMotion(int qp);

	static int GetLambdaModeFixedPoint(int qp);

	static int GetLambdaMotionFixedPoint(int qp);

	static int64_t CalculateRDCost(int64_t distortion, int64_t rate, int lambda);

};

__codec_end