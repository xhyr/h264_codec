#include "conformance_util.h"

#include "me_constant_values.h"

__codec_begin

int ConformanceUtil::GetLevelIndex(int profile_idc, int level_idc)
{
	switch (level_idc)
	{
	case 9:
		// we will accept level_idc equal to 9 for non-FRExt profiles, 
		// because it may be specified this way in the encoder config file
		return LEVEL_1b;
		break;
	case 10:
		return LEVEL_1;
		break;
	case 12:
		return LEVEL_1_2;
		break;
	case 13:
		return LEVEL_1_3;
		break;
	case 20:
		return LEVEL_2;
		break;
	case 21:
		return LEVEL_2_1;
		break;
	case 22:
		return LEVEL_2_2;
		break;
	case 30:
		return LEVEL_3;
		break;
	case 31:
		return LEVEL_3_1;
		break;
	case 32:
		return LEVEL_3_2;
		break;
	case 40:
		return LEVEL_4;
		break;
	case 41:
		return LEVEL_4_1;
		break;
	case 42:
		return LEVEL_4_2;
		break;
	case 50:
		return LEVEL_5;
		break;
	case 51:
		return LEVEL_5_1;
		break;
	case 52:
		return LEVEL_5_2;
		break;
	case 60:
		return LEVEL_6;
		break;
	case 61:
		return LEVEL_6_1;
		break;
	case 62:
		return LEVEL_6_2;
		break;
	case 0:
		return LEVEL_UNCONSTRAINED; // unconstrained experimental coding
		break;
	default:
		return 17;
		break;
	}
}

std::pair<int, int> ConformanceUtil::GetMVVerticalLimit(int profile_idc, int level_idc)
{
	int level_index = GetLevelIndex(profile_idc, level_idc);
	return MEConstantValues::mv_vertical_limts[level_index];
}

std::pair<int, int> ConformanceUtil::GetMVHorizontalLimit(int profile_idc, int level_idc)
{
	int level_index = GetLevelIndex(profile_idc, level_idc);
	return MEConstantValues::mv_horizontal_limts[level_index < 17 ? 0 : 1];
}

__codec_end