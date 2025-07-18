#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "global_defines.h"

__codec_begin

class Macroblock;

class LoopFilter
{
public:
	LoopFilter();
	~LoopFilter();

	void Filter(std::shared_ptr<Macroblock> mb);

private:
	void FilterVerticalEdges(std::shared_ptr<Macroblock> mb);

	void FilterHorizontalEdges(std::shared_ptr<Macroblock> mb);

	void FilterVerticalEdges(std::shared_ptr<Macroblock> mb, int edge);

	void FilterHorizontalEdges(std::shared_ptr<Macroblock> mb, int edge);

	void DoFilter(int strength, std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta);

	void DoFilterStrong(std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta);

	void DoFilterNormal(int strength, std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta);

	std::pair<int, int> CalculateAlphaAndBeta(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q);

	std::vector<uint8_t> GetLeftDataByVerticalEdge(std::shared_ptr<Macroblock> mb, int edge, uint32_t y);

	void SetLeftDataByVerticalEdge(std::shared_ptr<Macroblock> mb, int edge, uint32_t y, std::vector<uint8_t>& p_data);

	std::vector<uint8_t> GetRightDataByVerticalEdge(std::shared_ptr<Macroblock> mb, int edge, uint32_t y);

	void SetRightDataByVerticalEdge(std::shared_ptr<Macroblock> mb, int edge, uint32_t y, const std::vector<uint8_t>& q_data);

private:
	uint32_t m_mb_x;
	uint32_t m_mb_y;
};


__codec_end