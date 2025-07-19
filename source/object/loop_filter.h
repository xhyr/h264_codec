#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "common_types.h"

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

	void FilterVerticalEdgesLuma(std::shared_ptr<Macroblock> mb, int edge);

	void FilterHorizontalEdgesLuma(std::shared_ptr<Macroblock> mb, int edge);

	void FilterVerticalEdgesChroma(std::shared_ptr<Macroblock> mb, int edge, PlaneType plane_type);

	void FilterHorizontalEdgesChroma(std::shared_ptr<Macroblock> mb, int edge, PlaneType plane_type);

	bool DoFilter(int strength, std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta, int index_a, PlaneType plane_type);

	bool DoFilterStrongLuma(std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta);

	bool DoFilterStrongChroma(std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta);

	bool DoFilterNormalLuma(int strength, std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta, int index_a);

	bool DoFilterNormalChroma(int strength, std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta, int index_a);

	std::pair<int, int> CalculateAlphaAndBeta(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q, int& out_index_a);

	std::vector<uint8_t> GetLeftDataByVerticalEdge(std::shared_ptr<Macroblock> mb, const int edge, const uint32_t y, PlaneType plane_type);

	void SetLeftDataByVerticalEdge(std::shared_ptr<Macroblock> mb, const int edge, const uint32_t y, std::vector<uint8_t>& p_data, PlaneType plane_type);

	std::vector<uint8_t> GetRightDataByVerticalEdge(std::shared_ptr<Macroblock> mb, const int edge, const uint32_t y, PlaneType plane_type);

	void SetRightDataByVerticalEdge(std::shared_ptr<Macroblock> mb, const int edge, const uint32_t y, const std::vector<uint8_t>& q_data, PlaneType plane_type);

	std::vector<uint8_t> GetUpDataByHorizontalEdge(std::shared_ptr<Macroblock> mb, const int edge, const uint32_t x, PlaneType plane_type);

	void SetUpDataByHorizontalEdge(std::shared_ptr<Macroblock> mb, const int edge, const uint32_t x, std::vector<uint8_t>& p_data, PlaneType plane_type);

	std::vector<uint8_t> GetDownDataByHorizontalEdge(std::shared_ptr<Macroblock> mb, const int edge, const uint32_t x, PlaneType plane_type);

	void SetDownDataByHorizontalEdge(std::shared_ptr<Macroblock> mb, const int edge, const uint32_t x, const std::vector<uint8_t>& q_data, PlaneType plane_type);

private:
	uint32_t m_mb_x;
	uint32_t m_mb_y;
};


__codec_end