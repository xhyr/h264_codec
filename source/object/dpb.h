#pragma once

#include <cstdint>
#include <memory>
#include <deque>

#include "global_defines.h"

__codec_begin

struct YUVFrame;

class Dpb
{
public:
	explicit Dpb(uint32_t max_ref_frame_num);
	~Dpb();

	void InsertFrame(std::shared_ptr<YUVFrame> yuv_frame);

	std::shared_ptr<YUVFrame> GetFrame(uint32_t ref_id) const;

	uint32_t GetRefFrameNum() const;

private:
	uint32_t m_max_ref_frame_num;
	std::deque<std::shared_ptr<YUVFrame>> m_frames;
};

__codec_end