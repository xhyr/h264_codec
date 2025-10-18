#pragma once

#include <cstdint>
#include <vector>

#include "motion_info.h"

__codec_begin

class MotionInfoContext
{
public:
	MotionInfoContext(uint32_t frame_width_in_block, uint32_t frame_height_in_block);
	~MotionInfoContext();

	void SetMotionInfos(uint32_t mb_addr, uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block, uint32_t height_in_block, const MotionInfo& motion_info);
	
	void SetMotionInfos(uint32_t mb_addr, uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block, uint32_t height_in_block, const std::vector<MotionInfo>& motion_infos);

	MotionInfo GetMotionInfo(uint32_t mb_addr, uint32_t x_in_block, uint32_t y_in_block) const;

	std::vector<MotionInfo> GetMotionInfos(uint32_t mb_addr, uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block, uint32_t height_in_block) const;

	std::tuple<MotionInfo, MotionInfo, MotionInfo> GetNeighborMotionInfo(uint32_t x_in_block, uint32_t y_in_block, uint32_t width_in_block) const;

private:
	void Init();

private:
	uint32_t m_frame_width_in_block;
	uint32_t m_frame_height_in_block;
	std::vector<MotionInfo> m_motion_infos;
};

__codec_end