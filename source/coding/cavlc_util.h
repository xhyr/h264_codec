#pragma once

#include <cstdint>

#include "cavlc_types.h"

__codec_begin

struct CavlcUtil
{
	static uint8_t GetMaxCoeffNum(CavlcDataType data_type);

	static bool IsDCDataType(CavlcDataType data_type);

	static bool IsChromaDCDataType(CavlcDataType data_type);

	static bool IsLumaDataType(CavlcDataType data_type);

};

__codec_end