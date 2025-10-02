#include "loop_filter.h"

#include <algorithm>

#include "macroblock.h"
#include "filter_util.h"
#include "math_util.h"
#include "common_constant_values.h"
#include "filter_constant_values.h"

__codec_begin

LoopFilter::LoopFilter()
{
}

LoopFilter::~LoopFilter()
{
}

void LoopFilter::Filter(std::shared_ptr<Macroblock> mb)
{
	std::tie(m_mb_x, m_mb_y) = mb->GetPositionInMb();

	FilterVerticalEdges(mb);
	FilterHorizontalEdges(mb);
}

void LoopFilter::FilterVerticalEdges(std::shared_ptr<Macroblock> mb)
{
	bool filter_edge = m_mb_x > 0;

	//luma
	for (int edge = 0; edge < 4; ++edge)
	{
		if (filter_edge || edge > 0)
			FilterVerticalEdgesLuma(mb, edge);
	}

	//cb
	for (int edge = 0; edge < 2; ++edge)
	{
		if (filter_edge || edge > 0)
			FilterVerticalEdgesChroma(mb, edge, PlaneType::Cb);
	}

	//cr
	for (int edge = 0; edge < 2; ++edge)
	{
		if (filter_edge || edge > 0)
			FilterVerticalEdgesChroma(mb, edge, PlaneType::Cr);
	}
}

void LoopFilter::FilterHorizontalEdges(std::shared_ptr<Macroblock> mb)
{
	bool filter_edge = m_mb_y > 0;
	for (int edge = 0; edge < 4; ++edge)
	{
		if (filter_edge || edge > 0)
			FilterHorizontalEdgesLuma(mb, edge);
	}

	//cb
	for (int edge = 0; edge < 2; ++edge)
	{
		if (filter_edge || edge > 0)
			FilterHorizontalEdgesChroma(mb, edge, PlaneType::Cb);
	}

	//cr
	for (int edge = 0; edge < 2; ++edge)
	{
		if (filter_edge || edge > 0)
			FilterHorizontalEdgesChroma(mb, edge, PlaneType::Cr);
	}
}

void LoopFilter::FilterVerticalEdgesLuma(std::shared_ptr<Macroblock> mb, int edge)
{
	auto mb_p = edge == 0 ? mb->GetLeftMacroblock() : mb;
	auto mb_q = mb;

	int index_a;
	auto [alpha, beta] = CalculateAlphaAndBeta(mb_p, mb_q, index_a);

	if (alpha == 0 && beta == 0)
		return;

	auto strengths = FilterUtil::GetVerticalStrengths(mb_p, mb_q, edge);
	for (uint32_t y_in_block = 0; y_in_block < 4; ++y_in_block)
	{
		auto strength = strengths[y_in_block];
		if (strength == 0)
			continue;

		for (uint32_t y = 0; y < 4; ++y)
		{
			auto p_data = GetLeftDataByVerticalEdge(mb_p, edge, y + 4 * y_in_block, PlaneType::Luma);
			auto q_data = GetRightDataByVerticalEdge(mb_q, edge, y + 4 * y_in_block, PlaneType::Luma);
			bool changed = DoFilter(strength, p_data, q_data, alpha, beta, index_a, PlaneType::Luma);
			if (changed)
			{
				SetLeftDataByVerticalEdge(mb_p, edge, y + 4 * y_in_block, p_data, PlaneType::Luma);
				SetRightDataByVerticalEdge(mb_q, edge, y + 4 * y_in_block, q_data, PlaneType::Luma);
			}
		}
	}
}

void LoopFilter::FilterHorizontalEdgesLuma(std::shared_ptr<Macroblock> mb, int edge)
{
	auto mb_p = edge == 0 ? mb->GetUpMacroblock() : mb;
	auto mb_q = mb;

	int index_a;
	auto [alpha, beta] = CalculateAlphaAndBeta(mb_p, mb_q, index_a);

	if (alpha == 0 && beta == 0)
		return;

	auto strengths = FilterUtil::GetHorizontalStrengths(mb_p, mb_q, edge);
	for (uint32_t x_in_block = 0; x_in_block < 4; ++x_in_block)
	{
		auto strength = strengths[x_in_block];
		if (strength == 0)
			continue;

		for (uint32_t x = 0; x < 4; ++x)
		{
			auto p_data = GetUpDataByHorizontalEdge(mb_p, edge, x + 4 * x_in_block, PlaneType::Luma);
			auto q_data = GetDownDataByHorizontalEdge(mb_q, edge, x + 4 * x_in_block, PlaneType::Luma);
			bool changed = DoFilter(strength, p_data, q_data, alpha, beta, index_a, PlaneType::Luma);
			if (changed)
			{
				SetUpDataByHorizontalEdge(mb_p, edge, x + 4 * x_in_block, p_data, PlaneType::Luma);
				SetDownDataByHorizontalEdge(mb_q, edge, x + 4 * x_in_block, q_data, PlaneType::Luma);
			}
		}
	}
}

void LoopFilter::FilterVerticalEdgesChroma(std::shared_ptr<Macroblock> mb, int edge, PlaneType plane_type)
{
	auto mb_p = edge == 0 ? mb->GetLeftMacroblock() : mb;
	auto mb_q = mb;

	int index_a;
	auto [alpha, beta] = CalculateAlphaAndBeta(mb_p, mb_q, index_a);

	if (alpha == 0 && beta == 0)
		return;

	auto strengths = FilterUtil::GetVerticalStrengths(mb_p, mb_q, edge);
	for (uint32_t y_in_block = 0; y_in_block < 2; ++y_in_block)
	{
		auto strength = strengths[y_in_block * 2];
		if (strength == 0)
			continue;

		for (uint32_t y = 0; y < 4; ++y)
		{
			auto p_data = GetLeftDataByVerticalEdge(mb_p, edge, y + 4 * y_in_block, plane_type);
			auto q_data = GetRightDataByVerticalEdge(mb_q, edge, y + 4 * y_in_block, plane_type);
			bool changed = DoFilter(strength, p_data, q_data, alpha, beta, index_a, plane_type);
			if (changed)
			{
				SetLeftDataByVerticalEdge(mb_p, edge, y + 4 * y_in_block, p_data, plane_type);
				SetRightDataByVerticalEdge(mb_q, edge, y + 4 * y_in_block, q_data, plane_type);
			}
		}
	}
}

void LoopFilter::FilterHorizontalEdgesChroma(std::shared_ptr<Macroblock> mb, int edge, PlaneType plane_type)
{
	auto mb_p = edge == 0 ? mb->GetUpMacroblock() : mb;
	auto mb_q = mb;

	int index_a;
	auto [alpha, beta] = CalculateAlphaAndBeta(mb_p, mb_q, index_a);

	if (alpha == 0 && beta == 0)
		return;

	auto strengths = FilterUtil::GetVerticalStrengths(mb_p, mb_q, edge);
	for (uint32_t x_in_block = 0; x_in_block < 2; ++x_in_block)
	{
		auto strength = strengths[x_in_block * 2];
		if (strength == 0)
			continue;

		for (uint32_t x = 0; x < 4; ++x)
		{
			auto p_data = GetUpDataByHorizontalEdge(mb_p, edge, x + 4 * x_in_block, plane_type);
			auto q_data = GetDownDataByHorizontalEdge(mb_q, edge, x + 4 * x_in_block, plane_type);
			bool changed = DoFilter(strength, p_data, q_data, alpha, beta, index_a, plane_type);
			if (changed)
			{
				SetUpDataByHorizontalEdge(mb_p, edge, x + 4 * x_in_block, p_data, plane_type);
				SetDownDataByHorizontalEdge(mb_q, edge, x + 4 * x_in_block, q_data, plane_type);
			}
		}
	}
}

bool LoopFilter::DoFilter(int strength, std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta, int index_a, PlaneType plane_type)
{
	if (strength == 4)
		return plane_type == PlaneType::Luma ? DoFilterStrongLuma(p_data, q_data, alpha, beta) : DoFilterStrongChroma(p_data, q_data, alpha, beta);
	else
		return plane_type == PlaneType::Luma ? DoFilterNormalLuma(strength, p_data, q_data, alpha, beta, index_a) : DoFilterNormalChroma(strength, p_data, q_data, alpha, beta, index_a);
}

bool LoopFilter::DoFilterStrongLuma(std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta)
{
	auto old_p_data = p_data;
	auto old_q_data = q_data;
	
	int l0 = old_p_data[0];
	int r0 = old_q_data[0];
	int dif = r0 - l0;
	if (abs(dif) < alpha)
	{
		int l1 = old_p_data[1];
		int r1 = old_q_data[1];
		
		if (abs(r0 - r1) < beta && abs(l0 - l1) < beta)
		{
			int l2 = old_p_data[2];
			int r2 = old_q_data[2];
			int rl0 = l0 + r0;
			bool small_gap = abs(dif) < ((alpha >> 2) + 2);
			bool ap = small_gap && (abs(l0 - l2) < beta);
			bool aq = small_gap && (abs(r0 - r2) < beta);
			
			if (ap)
			{
				int l3 = old_p_data[3];
				p_data[0] = MathUtil::RightShift((r1 + ((l1 + rl0) << 1)) + l2, 3);
				p_data[1] = MathUtil::RightShift(l2 + l1 + rl0, 2);
				p_data[2] = MathUtil::RightShift(((l3 + l2) << 1) + l2 + l1 + rl0, 3);
			}
			else
			{
				p_data[0] = MathUtil::RightShift((l1 << 1) + l0 + r1, 2);
			}

			if (aq)
			{
				int r3 = old_q_data[3];
				q_data[0] = MathUtil::RightShift((l1 + ((r1 + rl0) << 1)) + r2, 3);
				q_data[1] = MathUtil::RightShift(r2 + r1 + rl0, 2);
				q_data[2] = MathUtil::RightShift(((r3 + r2) << 1) + r2 + r1 + rl0, 3);
			}
			else
			{
				q_data[0] = MathUtil::RightShift((r1 << 1) + r0 + l1, 2);
			}
		}
	}

	return !(std::equal(p_data.begin(), p_data.end(), old_p_data.begin(), old_p_data.end()) && std::equal(q_data.begin(), q_data.end(), old_q_data.begin(), old_q_data.end()));
}

bool LoopFilter::DoFilterStrongChroma(std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta)
{
	auto old_p_data = p_data;
	auto old_q_data = q_data;

	int l0 = old_p_data[0];
	int r0 = old_q_data[0];
	if (abs(r0 - l0) < alpha)
	{
		int l1 = old_p_data[1];
		int r1 = old_q_data[1];

		if (abs(r0 - r1) < beta && abs(l0 - l1) < beta)
		{
			p_data[0] = MathUtil::RightShift((l1 << 1) + l0 + r1, 2);
			q_data[0] = MathUtil::RightShift((r1 << 1) + r0 + l1, 2);
		}
	}

	return !(std::equal(p_data.begin(), p_data.end(), old_p_data.begin(), old_p_data.end()) && std::equal(q_data.begin(), q_data.end(), old_q_data.begin(), old_q_data.end()));
}

bool LoopFilter::DoFilterNormalLuma(int strength, std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta, int index_a)
{
	auto old_p_data = p_data;
	auto old_q_data = q_data;

	int c0 = FilterConstantValues::s_clip_table[index_a][strength];

	int l0 = old_p_data[0];
	int r0 = old_q_data[0];
	int edge_diff = r0 - l0;
	if (abs(edge_diff) < alpha)
	{
		int l1 = old_p_data[1];
		int r1 = old_q_data[1];
		
		if (abs(r0 - r1) < beta && abs(l0 - l1) < beta)
		{
			int rl0 = MathUtil::RightShift(l0 + r0, 1);
			int l2 = old_p_data[2];
			int r2 = old_q_data[2];
			
			int ap = abs(l0 - l2) < beta;
			int aq = abs(r0 - r2) < beta;
			
			int tc0 = c0 + ap + aq;
			int dif = MathUtil::Clamp(MathUtil::RightShift((edge_diff << 2) + (l1 - r1), 3), -tc0, tc0);

			if (dif != 0)
			{
				p_data[0] = MathUtil::Clamp<int>(l0 + dif, CommonConstantValues::s_min_color_value, CommonConstantValues::s_max_color_value);
				q_data[0] = MathUtil::Clamp<int>(r0 - dif, CommonConstantValues::s_min_color_value, CommonConstantValues::s_max_color_value);
			}

			if(ap != 0)
				p_data[1] = l1 + MathUtil::Clamp<int>((l2 + rl0 -(l1 << 1)) >> 1, -c0, c0);

			if (aq != 0)
				q_data[1] = r1 + MathUtil::Clamp<int>((r2 + rl0 - (r1 << 1)) >> 1, -c0, c0);
		}
	}

	return !(std::equal(p_data.begin(), p_data.end(), old_p_data.begin(), old_p_data.end()) && std::equal(q_data.begin(), q_data.end(), old_q_data.begin(), old_q_data.end()));
}

bool LoopFilter::DoFilterNormalChroma(int strength, std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta, int index_a)
{
	auto old_p_data = p_data;
	auto old_q_data = q_data;

	int c0 = FilterConstantValues::s_clip_table[index_a][strength];

	int l0 = old_p_data[0];
	int r0 = old_q_data[0];
	int edge_diff = r0 - l0;
	if (abs(edge_diff) < alpha)
	{
		int l1 = old_p_data[1];
		int r1 = old_q_data[1];

		if (abs(r0 - r1) < beta && abs(l0 - l1) < beta)
		{
			int tc0 = c0 + 1;
			int dif = MathUtil::Clamp(MathUtil::RightShift((edge_diff << 2) + (l1 - r1), 3), -tc0, tc0);

			if (dif != 0)
			{
				p_data[0] = MathUtil::Clamp<int>(l0 + dif, CommonConstantValues::s_min_color_value, CommonConstantValues::s_max_color_value);
				q_data[0] = MathUtil::Clamp<int>(r0 - dif, CommonConstantValues::s_min_color_value, CommonConstantValues::s_max_color_value);
			}
		}
	}

	return !(std::equal(p_data.begin(), p_data.end(), old_p_data.begin(), old_p_data.end()) && std::equal(q_data.begin(), q_data.end(), old_q_data.begin(), old_q_data.end()));
}

std::pair<int, int> LoopFilter::CalculateAlphaAndBeta(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q, int& out_index_a)
{
	int average_qp = MathUtil::RightShift(mb_p->GetQP() + mb_q->GetQP(), 1);

	int index_a = MathUtil::Clamp<int>(average_qp, CommonConstantValues::s_min_qp, CommonConstantValues::s_max_qp);
	int index_b = MathUtil::Clamp<int>(average_qp, CommonConstantValues::s_min_qp, CommonConstantValues::s_max_qp);
	
	int alpha = FilterConstantValues::s_alpha_table[index_a];
	int beta = FilterConstantValues::s_beta_table[index_b];

	out_index_a = index_a;
	return std::make_pair(alpha, beta);
}

std::vector<uint8_t> LoopFilter::GetLeftDataByVerticalEdge(std::shared_ptr<Macroblock> mb, const int edge, const uint32_t y, PlaneType plane_type)
{
	std::vector<uint8_t> ret(4);
	if (plane_type == PlaneType::Luma)
	{
		auto mb_data = mb->GetReconstructedLumaBlockData();
		int start_x = (edge * 4 - 1 + 16) % 16;
		int finish_x = start_x - 3;;
		for (int x = start_x; x >= finish_x; --x)
			ret[start_x - x] = mb_data.GetElement(x, y);
	}
	else
	{
		auto mb_data = mb->GetReconstructedChromaBlockData(plane_type);
		int start_x = (edge * 4 -1 + 8) % 8;
		int finish_x = start_x - 3;
		for (int x = start_x; x >= finish_x; --x)
			ret[start_x - x] = mb_data.GetElement(x, y);
	}
	return ret;
}

void LoopFilter::SetLeftDataByVerticalEdge(std::shared_ptr<Macroblock> mb, const int edge, const uint32_t y, std::vector<uint8_t>& p_data, PlaneType plane_type)
{
	if (plane_type == PlaneType::Luma)
	{
		auto mb_data = mb->GetReconstructedLumaBlockData();
		int start_x = (edge * 4 - 1 + 16) % 16;
		int finish_x = start_x - 3;;
		for (int x = start_x; x >= finish_x; --x)
			mb_data.SetElement(x, y, p_data[start_x - x]);
		mb->SetReconstructedLumaBlockData(mb_data);
	}
	else
	{
		auto mb_data = mb->GetReconstructedChromaBlockData(plane_type);
		int start_x = (edge * 4 - 1 + 8) % 8;
		int finish_x = start_x - 3;
		for (int x = start_x; x >= finish_x; --x)
			mb_data.SetElement(x, y, p_data[start_x - x]);
		mb->SetReconstructedChromaBlockData(mb_data, plane_type);
	}
}

std::vector<uint8_t> LoopFilter::GetRightDataByVerticalEdge(std::shared_ptr<Macroblock> mb, const int edge, const uint32_t y, PlaneType plane_type)
{
	std::vector<uint8_t> ret(4);
	if (plane_type == PlaneType::Luma)
	{
		auto mb_data = mb->GetReconstructedLumaBlockData();
		int start_x = edge * 4;
		int finish_x = edge * 4 + 3;
		for (int x = start_x; x <= finish_x; ++x)
			ret[x - start_x] = mb_data.GetElement(x, y);
	}
	else
	{
		auto mb_data = mb->GetReconstructedChromaBlockData(plane_type);
		int start_x = edge * 4;
		int finish_x = edge * 4 + 3;
		for (int x = start_x; x <= finish_x; ++x)
			ret[x - start_x] = mb_data.GetElement(x, y);
	}

	return ret;
}

void LoopFilter::SetRightDataByVerticalEdge(std::shared_ptr<Macroblock> mb, const int edge, const uint32_t y, const std::vector<uint8_t>& q_data, PlaneType plane_type)
{
	if (plane_type == PlaneType::Luma)
	{
		auto mb_data = mb->GetReconstructedLumaBlockData();
		int start_x = edge * 4;
		int finish_x = edge * 4 + 3;
		for (int x = start_x; x <= finish_x; ++x)
			mb_data.SetElement(x, y, q_data[x - start_x]);
		mb->SetReconstructedLumaBlockData(mb_data);
	}
	else
	{
		auto mb_data = mb->GetReconstructedChromaBlockData(plane_type);
		int start_x = edge * 4;
		int finish_x = edge * 4 + 3;
		for (int x = start_x; x <= finish_x; ++x)
			mb_data.SetElement(x, y, q_data[x - start_x]);
		mb->SetReconstructedChromaBlockData(mb_data, plane_type);
	}
}

std::vector<uint8_t> LoopFilter::GetUpDataByHorizontalEdge(std::shared_ptr<Macroblock> mb, const int edge, const uint32_t x, PlaneType plane_type)
{
	std::vector<uint8_t> ret(4);
	if (plane_type == PlaneType::Luma)
	{
		auto mb_data = mb->GetReconstructedLumaBlockData();
		int start_y = (edge * 4 - 1 + 16) % 16;
		int finish_y = start_y - 3;
		for (int y = start_y; y >= finish_y; --y)
			ret[start_y - y] = mb_data.GetElement(x, y);
	}
	else
	{
		auto mb_data = mb->GetReconstructedChromaBlockData(plane_type);
		int start_y = (edge * 4 - 1 + 8) % 8;
		int finish_y = start_y - 3;
		for (int y = start_y; y >= finish_y; --y)
			ret[start_y - y] = mb_data.GetElement(x, y);
	}
	return ret;
}

void LoopFilter::SetUpDataByHorizontalEdge(std::shared_ptr<Macroblock> mb, const int edge, const uint32_t x, std::vector<uint8_t>& p_data, PlaneType plane_type)
{
	if (plane_type == PlaneType::Luma)
	{
		auto mb_data = mb->GetReconstructedLumaBlockData();
		int start_y = (edge * 4 - 1 + 16) % 16;
		int finish_y = start_y - 3;
		for (int y = start_y; y >= finish_y; --y)
			mb_data.SetElement(x, y, p_data[start_y - y]);
		mb->SetReconstructedLumaBlockData(mb_data);
	}
	else
	{
		auto mb_data = mb->GetReconstructedChromaBlockData(plane_type);
		int start_y = (edge * 4 - 1 + 8) % 8;
		int finish_y = start_y - 3;
		for (int y = start_y; y >= finish_y; --y)
			mb_data.SetElement(x, y, p_data[start_y - y]);
		mb->SetReconstructedChromaBlockData(mb_data, plane_type);
	}
}

std::vector<uint8_t> LoopFilter::GetDownDataByHorizontalEdge(std::shared_ptr<Macroblock> mb, const int edge, const uint32_t x, PlaneType plane_type)
{
	std::vector<uint8_t> ret(4);
	if (plane_type == PlaneType::Luma)
	{
		auto mb_data = mb->GetReconstructedLumaBlockData();
		int start_y = edge * 4;
		int finish_y = start_y + 3;
		for (int y = start_y; y <= finish_y; ++y)
			ret[y - start_y] = mb_data.GetElement(x, y);
	}
	else
	{
		auto mb_data = mb->GetReconstructedChromaBlockData(plane_type);
		int start_y = edge * 4;
		int finish_y = start_y + 3;
		for (int y = start_y; y <= finish_y; ++y)
			ret[y - start_y] = mb_data.GetElement(x, y);
	}
	return ret;
}

void LoopFilter::SetDownDataByHorizontalEdge(std::shared_ptr<Macroblock> mb, int edge, uint32_t x, const std::vector<uint8_t>& q_data, PlaneType plane_type)
{
	if (plane_type == PlaneType::Luma)
	{
		auto mb_data = mb->GetReconstructedLumaBlockData();
		int start_y = edge * 4;
		int finish_y = start_y + 3;
		for (int y = start_y; y <= finish_y; ++y)
			mb_data.SetElement(x, y, q_data[y - start_y]);
		mb->SetReconstructedLumaBlockData(mb_data);
	}
	else
	{
		auto mb_data = mb->GetReconstructedChromaBlockData(plane_type);
		int start_y = edge * 4;
		int finish_y = start_y + 3;
		for (int y = start_y; y <= finish_y; ++y)
			mb_data.SetElement(x, y, q_data[y - start_y]);
		mb->SetReconstructedChromaBlockData(mb_data, plane_type);
	}
}

__codec_end

