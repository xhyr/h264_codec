#include "rdo_util.h"

#include <algorithm>

#include "rdo_constant_values.h"

__codec_begin

double RDOUtil::GetLambda(int qp)
{
	return RDOConstantValues::s_qp2quant[std::max(0, qp - RDOConstantValues::s_qp_shift)];
}

__codec_end


