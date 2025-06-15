#pragma once

#include <string>

#include "codec_types.h"

__codec_begin

struct CodecTypesFormater
{
	static std::string FormatNaluType(NaluType nalu_type);

	static std::string FormatNaluPriority(NaluPriority nalu_priority);
};

__codec_end