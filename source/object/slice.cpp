#include "slice.h"

#include "sps.h"
#include "nalu.h"
#include "encoder_context.h"
#include "macroblock.h"
#include "log.h"
#include "cavlc_context.h"
#include "bytes_data.h"

__codec_begin

SliceType Slice::GetType() const
{
	return m_type;
}

void Slice::Construct(uint32_t tick, SliceType slice_type, std::shared_ptr<SPS> sps, std::shared_ptr<PPS> pps)
{
	m_type = slice_type;
	m_header.Construct(tick, slice_type == SliceType::I, static_cast<uint32_t>(slice_type), sps->GetData());
}

bool Slice::Encode(std::shared_ptr<EncoderContext> encoder_context)
{
	m_cavlc_context = std::make_shared<CavlcContext>(encoder_context->width_in_mb, encoder_context->height_in_mb);

	m_bytes_data = m_header.Convert2BytesData();

	for (uint32_t mb_addr = 0; mb_addr < encoder_context->mb_num; ++mb_addr)
	{
		auto macroblock = std::make_shared<Macroblock>(mb_addr, weak_from_this(), encoder_context);
		auto old_bit_count = m_bytes_data->GetBitsCount();

		if (mb_addr == 55)
			int sb = 1;

		macroblock->Encode(m_bytes_data);
		auto used_bit_count = m_bytes_data->GetBitsCount() - old_bit_count;

		LOGINFO("%d, used_bit_count = %d.", mb_addr, used_bit_count);

		m_cost += macroblock->GetCost();
		m_macroblocks.push_back(macroblock);
	}

	return true;
}

void Slice::Serial(std::shared_ptr<OStream> ostream)
{
	Nalu nalu(NaluType::IDR, NaluPriority::HIGHEST);
	
	//header
	nalu.SetData(m_bytes_data);

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
