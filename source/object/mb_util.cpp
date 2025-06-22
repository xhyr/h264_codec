#include "mb_util.h"

#include "encoder_context.h"

__codec_begin

bool MBUtil::IsAvailable(uint32_t mb_addr, std::shared_ptr<EncoderContext> encoder_context)
{
	return mb_addr < encoder_context->mb_num;
}

__codec_end


