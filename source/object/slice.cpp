#include "slice.h"

#include "sps.h"
#include "nalu.h"
#include "encoder_context.h"
#include "macroblock.h"
#include "log.h"
#include "cavlc_context.h"
#include "bytes_data.h"
#include "encoder_config.h"
#include "loop_filter.h"
#include "yuv_frame.h"
#include "data_util.h"

__codec_begin

SliceType Slice::GetType() const
{
	return m_type;
}

void Slice::Construct(uint32_t tick, SliceType slice_type, std::shared_ptr<SPS> sps, std::shared_ptr<PPS> pps, std::shared_ptr<EncoderContext> encoder_context)
{
	m_tick = tick;
	m_type = slice_type;
	m_qp = encoder_context->config->qp;
	m_encoder_context = encoder_context;
	m_header.Construct(m_tick, m_tick == 0 && slice_type == SliceType::I, static_cast<uint32_t>(slice_type), sps->GetData(), m_qp);
}

bool Slice::Encode()
{
	m_frame_data = std::make_shared<YUVFrame>(m_encoder_context->width, m_encoder_context->height);
	m_bytes_data = m_header.Convert2BytesData();

	auto header_bit_count = m_bytes_data->GetBitsCount();

	uint32_t total_used_bit_count = 0;
	for (uint32_t mb_addr = 0; mb_addr < m_encoder_context->mb_num; ++mb_addr)
	{
		auto mb = std::make_shared<Macroblock>(mb_addr, weak_from_this(), m_encoder_context);
		auto old_bit_count = m_bytes_data->GetBitsCount();

		if (m_encoder_context->slice_addr == 1)
			int sb = 1;

		mb->Encode(m_bytes_data);
		auto used_bit_count = m_bytes_data->GetBitsCount() - old_bit_count;

		total_used_bit_count += used_bit_count;

		if (m_encoder_context->slice_addr == 1)
			LOGINFO("mb_addr = %d, used_bits = %d, total_used_byte_count = %d.", mb_addr, used_bit_count, total_used_bit_count / 8);

		m_macroblocks.push_back(mb);

		CollectFrameData(mb);
	}

	LOGINFO("tick = %d, header_bit_count = %d.", m_tick, header_bit_count);

	DeblockFilter();

	CollectAllFrameData();

	AddToDPB();

	m_macroblocks.clear();

	return true;
}

void Slice::Serial(std::shared_ptr<OStream> ostream)
{
	Nalu nalu(m_tick == 0 ? NaluType::IDR : NaluType::Slice, m_tick == 0 ? NaluPriority::HIGHEST : NaluPriority::HIGH);
	
	nalu.SetData(m_bytes_data);

	//write out
	nalu.Serial(ostream);
}

std::shared_ptr<Macroblock> Slice::GetMacroblock(uint32_t mb_addr)
{
	return m_macroblocks[mb_addr];
}

int Slice::GetQP() const
{
	return m_qp;
}

std::shared_ptr<YUVFrame> Slice::GetFrameData()
{
	return m_frame_data;
}

void Slice::RefreshBlockData(uint32_t mb_addr, uint32_t x_in_block, uint32_t y_in_block, const BlockData<4, 4>& block_data)
{
	x_in_block = (mb_addr % m_encoder_context->width_in_mb) * 4 + x_in_block;
	y_in_block = (mb_addr / m_encoder_context->width_in_mb) * 4 + y_in_block;
	DataUtil::SetData(m_frame_data->y_data, block_data.Convert2DataPtr(), x_in_block * 4, y_in_block * 4, 4, 4, m_encoder_context->width);
}

void Slice::DeblockFilter()
{
	if (m_header.disable_deblocking_filter_idc == 1)
		return;

	LoopFilter loop_filter;
	for (auto mb : m_macroblocks)
		loop_filter.Filter(mb);
}

void Slice::CollectFrameData(std::shared_ptr<Macroblock> mb)
{
	auto [x_in_mb, y_in_mb] = mb->GetPositionInMb();
	auto luma_data = mb->GetReconstructedLumaBlockData();
	DataUtil::SetData(m_frame_data->y_data, luma_data.Convert2DataPtr(), x_in_mb * 16, y_in_mb * 16, 16, 16, m_encoder_context->width);

	auto cb_data = mb->GetReconstructedChromaBlockData(PlaneType::Cb);
	DataUtil::SetData(m_frame_data->u_data, cb_data.Convert2DataPtr(), x_in_mb * 8, y_in_mb * 8, 8, 8, m_encoder_context->width / 2);

	auto cr_data = mb->GetReconstructedChromaBlockData(PlaneType::Cr);
	DataUtil::SetData(m_frame_data->v_data, cr_data.Convert2DataPtr(), x_in_mb * 8, y_in_mb * 8, 8, 8, m_encoder_context->width / 2);
}

void Slice::CollectAllFrameData()
{
	for (auto mb : m_macroblocks)
		CollectFrameData(mb);
}

void Slice::AddToDPB()
{
	m_encoder_context->last_frame = m_frame_data;
}

__codec_end
