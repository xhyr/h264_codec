#include "rdo_util.h"

#include <algorithm>

#include "rdo_constant_values.h"

__codec_begin

double RDOUtil::GetLambdaMode(int qp)
{
	return 0.85 * pow(2, (qp - RDOConstantValues::s_qp_shift) / 3.0);
}

double RDOUtil::GetLambdaMotion(int qp)
{
	return sqrt(GetLambdaMode(qp));
}

__codec_end