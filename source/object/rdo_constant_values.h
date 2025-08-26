#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

struct RDOConstantValues
{
	constexpr static int s_qp_shift{ 12 };

	constexpr static int s_qp2quant[]
	{
		 1, 1, 1, 1, 2, 2, 2, 2,
		 3, 3, 3, 4, 4, 4, 5, 6,
		 6, 7, 8, 9,10,11,13,14,
	     16,18,20,23,25,29,32,36,
		 40,45,51,57,64,72,81,91
	};

	constexpr static int s_lambda_accuracy_bits{ 5 };
};


__codec_end