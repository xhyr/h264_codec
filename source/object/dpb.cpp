#include "dpb.h"

__codec_begin

Dpb::Dpb(uint32_t max_ref_frame_num) : m_max_ref_frame_num(max_ref_frame_num)
{
}

Dpb::~Dpb()
{
}

void Dpb::InsertFrame(std::shared_ptr<YUVFrame> yuv_frame)
{
	m_frames.emplace_front(yuv_frame);
}

uint32_t Dpb::GetRefFrameNum() const
{
	return m_frames.size();
}

std::shared_ptr<YUVFrame> Dpb::GetFrame(uint32_t ref_id) const
{
	if (ref_id < 0 || ref_id >= m_frames.size())
		return nullptr;

	return m_frames[ref_id];
}

__codec_end