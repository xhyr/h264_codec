#include "codec_types_formater.h"

__codec_begin

std::string CodecTypesFormater::FormatNaluType(NaluType nalu_type)
{
	std::string ret;
	switch (nalu_type)
	{
	case NaluType::IDR:
		ret = "IDR";
		break;
	case NaluType::SPS:
		ret = "SPS";
		break;
	case NaluType::PPS:
		ret = "PPS";
		break;
	default:
		ret = "Unknown";
		break;
	}
	return ret;
}

std::string CodecTypesFormater::FormatNaluPriority(NaluPriority nalu_priority)
{
	std::string ret;
	switch (nalu_priority)
	{
	case NaluPriority::DISPOSABLE:
		ret = "Disposable";
		break;
	case NaluPriority::LOW:
		ret = "Low";
		break;
	case NaluPriority::HIGH:
		ret = "High";
		break;
	case NaluPriority::HIGHEST:
		ret = "Highest";
		break;
	default:
		ret = "Unknown";
		break;
	}
	return ret;
}


__codec_end

