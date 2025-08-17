#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

struct RDOUtil
{
	static double GetLambdaMode(int qp);

	static double GetLambdaMotion(int qp);

};

__codec_end