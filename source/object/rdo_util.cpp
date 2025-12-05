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

int RDOUtil::GetLambdaModeFixedPoint(int qp)
{
	auto lambda = GetLambdaMode(qp);
	return  static_cast<int>((1 << RDOConstantValues::s_lambda_accuracy_bits) * lambda + 0.5);
}

int RDOUtil::GetLambdaMotionFixedPoint(int qp)
{
	auto lambda = GetLambdaMotion(qp);
	return  static_cast<int>((1 << RDOConstantValues::s_lambda_accuracy_bits) * lambda + 0.5);
}

int64_t RDOUtil::CalculateRDCost(int64_t distortion, int64_t rate, int lambda)
{
	return distortion + (rate > 0 ? lambda * rate : lambda * 1 / 2);
}

__codec_end