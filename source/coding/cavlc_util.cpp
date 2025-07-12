#include "cavlc_util.h"

__codec_begin

uint8_t CavlcUtil::GetMaxCoeffNum(CavlcDataType data_type)
{
	uint8_t max_coeff_num = 0;
	switch (data_type)
	{
	case codec::CavlcDataType::NormalLuma:
	case codec::CavlcDataType::LumaDC:
		max_coeff_num = 16;
		break;
	case codec::CavlcDataType::LumaAC:
		max_coeff_num = 15;
		break;
	case codec::CavlcDataType::CbDC:
	case codec::CavlcDataType::CrDC:
		max_coeff_num = 4;
		break;
	case codec::CavlcDataType::CbAC:
	case codec::CavlcDataType::CrAC:
		max_coeff_num = 15;
		break;
	default:
		break;
	}
	return max_coeff_num;
}

bool CavlcUtil::IsDCDataType(CavlcDataType data_type)
{
	return data_type == CavlcDataType::LumaDC || data_type == CavlcDataType::CbDC || data_type == CavlcDataType::CrDC;
}

bool CavlcUtil::IsChromaDCDataType(CavlcDataType data_type)
{
	return data_type == CavlcDataType::CbDC || data_type == CavlcDataType::CrDC;
}

bool CavlcUtil::IsLumaDataType(CavlcDataType data_type)
{
	return data_type == CavlcDataType::NormalLuma || data_type == CavlcDataType::LumaDC || data_type == CavlcDataType::LumaAC;
}

__codec_end