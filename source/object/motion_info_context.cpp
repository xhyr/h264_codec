#include "motion_info_context.h"

#include "block_util.h"

__codec_begin

MotionInfoContext::MotionInfoContext(uint32_t frame_width_in_block, uint32_t frame_height_in_block) : 
	m_frame_width_in_block(frame_width_in_block),
	m_frame_height_in_block(frame_height_in_block),
	m_motion_infos(frame_width_in_block * frame_height_in_block)
{
	Init();
}

MotionInfoContext::~MotionInfoContext()
{
}

void MotionInfoContext::SetMotionInfos(uint32_t mb_addr, uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block, uint32_t height_in_block, const MotionInfo& motion_info)
{
	auto block_indices = BlockUtil::GetBlockIndices(mb_addr, x_in_block, y_in_block, width_in_block, height_in_block, m_frame_width_in_block);
	for (auto block_index : block_indices)
		m_motion_infos[block_index] = motion_info;
}

void MotionInfoContext::SetMotionInfos(uint32_t mb_addr, uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block, uint32_t height_in_block, const std::vector<MotionInfo>& motion_infos)
{
	auto block_indices = BlockUtil::GetBlockIndices(mb_addr, x_in_block, y_in_block, width_in_block, height_in_block, m_frame_width_in_block);
	uint32_t index = 0;
	for (auto block_index : block_indices)
		m_motion_infos[block_index] = motion_infos[index++];
}

MotionInfo MotionInfoContext::GetMotionInfo(uint32_t mb_addr, uint32_t x_in_block, uint32_t y_in_block) const
{
	auto block_indices = BlockUtil::GetBlockIndices(mb_addr, x_in_block, y_in_block, 1, 1, m_frame_width_in_block);
	return m_motion_infos[block_indices[0]];
}

std::vector<MotionInfo> MotionInfoContext::GetMotionInfos(uint32_t mb_addr, uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block, uint32_t height_in_block) const
{
	auto block_indices = BlockUtil::GetBlockIndices(mb_addr, x_in_block, y_in_block, width_in_block, height_in_block, m_frame_width_in_block);
	std::vector<MotionInfo> motion_infos;
	motion_infos.reserve(block_indices.size());
	for (auto block_index : block_indices)
		motion_infos.push_back(m_motion_infos[block_index]);
	return motion_infos;
}

std::tuple<MotionInfo, MotionInfo, MotionInfo> MotionInfoContext::GetNeighborMotionInfo(uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block) const
{
	uint32_t block_index;
	//a
	auto got = BlockUtil::GetBlockNeighborIndex(x_in_block, y_in_block, width_in_block, m_frame_width_in_block, BlockNeighborType::A, block_index);
	MotionInfo motion_info_a;
	if (got)
		motion_info_a = m_motion_infos[block_index];

	//b
	got = BlockUtil::GetBlockNeighborIndex(x_in_block, y_in_block, width_in_block, m_frame_width_in_block, BlockNeighborType::B, block_index);
	MotionInfo motion_info_b;
	if (got)
		motion_info_b = m_motion_infos[block_index];

	//c
	got = BlockUtil::GetBlockNeighborIndex(x_in_block, y_in_block, width_in_block, m_frame_width_in_block, BlockNeighborType::C, block_index);
	MotionInfo motion_info_c;
	if (got)
	{
		motion_info_c = m_motion_infos[block_index];
	}
	else
	{
		got = BlockUtil::GetBlockNeighborIndex(x_in_block, y_in_block, width_in_block, m_frame_width_in_block, BlockNeighborType::D, block_index);
		if (got)
			motion_info_c = m_motion_infos[block_index];
	}

	return std::make_tuple(motion_info_a, motion_info_b, motion_info_c);
}

void MotionInfoContext::RestMotionInfos(uint32_t mb_addr)
{
	MotionInfo motion_info;
	SetMotionInfos(mb_addr, 0, 0, 4, 4, motion_info);
}

void MotionInfoContext::Init()
{
	
}

__codec_end


