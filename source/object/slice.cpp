#include "slice.h"

#include "sps.h"
#include "nalu.h"
#include "encoder_context.h"
#include "macroblock.h"
#include "yuv_frame.h"
#include "png_predictor.h"
#include "log.h"
#include "cavlc_context.h"

__codec_begin

SliceType Slice::GetType() const
{
	return m_type;
}

void Slice::Construct(SliceType slice_type, std::shared_ptr<SPS> sps, std::shared_ptr<PPS> pps)
{
	m_type = slice_type;
	m_header.Construct(slice_type == SliceType::I, static_cast<uint32_t>(slice_type), sps->GetData());
}

bool Slice::Encode(std::shared_ptr<EncoderContext> encoder_context)
{
	m_cavlc_context = std::make_shared<CavlcContext>(encoder_context->width_in_mb, encoder_context->height_in_mb);

	for (uint32_t mb_addr = 0; mb_addr < encoder_context->mb_num; ++mb_addr)
	{
		auto macroblock = std::make_shared<Macroblock>(mb_addr, weak_from_this(), encoder_context);
		macroblock->Encode();
		m_cost += macroblock->GetCost();
		m_macroblocks.push_back(macroblock);
	}

	return true;
}

void Slice::Serial(std::shared_ptr<OStream> ostream)
{
	Nalu nalu(NaluType::IDR, NaluPriority::HIGHEST);
	
	//header
	nalu.SetData(m_header.Convert2BytesData());

	//write out
	nalu.Serial(ostream);
}

std::shared_ptr<Macroblock> Slice::GetMacroblock(uint32_t mb_addr)
{
	return m_macroblocks[mb_addr];
}

int Slice::GetCost() const
{
	return m_cost;
}

std::shared_ptr<CavlcContext> Slice::GetCavlcContext()
{
    return m_cavlc_context;
}

__codec_end
