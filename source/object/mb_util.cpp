#include "mb_util.h"

#include "encoder_context.h"

__codec_begin

bool MBUtil::IsAvailable(uint32_t mb_addr, std::shared_ptr<EncoderContext> encoder_context)
{
	return mb_addr < encoder_context->mb_num;
}

int MBUtil::CalculateIntra16Offset(uint8_t cbp, Intra16LumaPredictionType prediction_type)
{
    return (cbp & 15 ? 13 : 1) + (int)prediction_type + ((cbp & 0x30) >> 2);
}

__codec_end


