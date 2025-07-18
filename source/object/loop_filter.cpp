#include "loop_filter.h"

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

	for (int edge = 0; edge < 4; ++edge)
	{
		if (filter_edge || edge > 0)
			FilterVerticalEdges(mb, edge);
	}
}

void LoopFilter::FilterHorizontalEdges(std::shared_ptr<Macroblock> mb)
{

}

void LoopFilter::FilterVerticalEdges(std::shared_ptr<Macroblock> mb, int edge)
{
	auto mb_p = edge == 0 ? mb->GetLeftMacroblock() : mb;
	auto mb_q = mb;

	auto [alpha, beta] = CalculateAlphaAndBeta(mb_p, mb_q);

	if (alpha == 0 && beta == 0)
		return;

	auto mb_p_data = mb_p->GetReconstructedLumaBlockData();
	auto mb_q_data = mb_q->GetReconstructedLumaBlockData();

	auto strengths = FilterUtil::GetVerticalStrengths(mb, edge);
	for (uint32_t y_in_block = 0; y_in_block < 4; ++y_in_block)
	{
		auto strength = strengths[y_in_block];
		if (strength == 0)
			continue;

		for (uint32_t y = 0; y < 4; ++y)
		{
			auto p_data = GetLeftDataByVerticalEdge(mb_p, edge, y);
			auto q_data = GetRightDataByVerticalEdge(mb_q, edge, y);
			DoFilter(strength, p_data, q_data, alpha, beta);
			SetLeftDataByVerticalEdge(mb_p, edge, y, p_data);
			SetRightDataByVerticalEdge(mb_q, edge, y, q_data);
		}
	}
}

void LoopFilter::FilterHorizontalEdges(std::shared_ptr<Macroblock> mb, int edge)
{
}

void LoopFilter::DoFilter(int strength, std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta)
{
	if (strength == 4)
		DoFilterStrong(p_data, q_data, alpha, beta);
	else
		DoFilterNormal(strength, p_data, q_data, alpha, beta);
}

void LoopFilter::DoFilterStrong(std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta)
{
	auto old_p_data = p_data;
	auto old_q_data = q_data;
	
	auto l0 = old_p_data[0];
	auto r0 = old_q_data[0];
	if (abs(r0 - l0) < alpha)
	{
		auto l1 = old_p_data[1];
		auto r1 = old_q_data[1];
		
		if (abs(r0 - r1) < beta && abs(l0 - l1) < beta)
		{
			auto l2 = old_p_data[2];
			auto r2 = old_q_data[2];
			int rl0 = l0 + r0;
			bool small_gap = abs(r0 - l0) < (alpha >> 2) + 2;
			bool ap = small_gap && (abs(l0 - l2) < beta);
			bool aq = small_gap && (abs(r0 - r2) < beta);
			
			if (ap)
			{
				auto l3 = old_p_data[3];
				p_data[0] = MathUtil::RightShift((r1 + ((l1 + rl0) << 2)) + l2, 3);
				p_data[1] = MathUtil::RightShift(l2 + l1 + rl0, 2);
				p_data[2] = MathUtil::RightShift(((l3 + l2) << 1) + l2 + l1 + rl0, 3);
			}
			else
			{
				p_data[0] = MathUtil::RightShift((l1 << 1) + l0 + r1, 2);
			}

			if (aq)
			{
				auto r3 = old_q_data[3];
				q_data[0] = MathUtil::RightShift((l1 + ((r1 + rl0) << 2)) + r2, 3);
				q_data[1] = MathUtil::RightShift(r2 + r1 + rl0, 2);
				q_data[2] = MathUtil::RightShift(((r3 + r2) << 1) + r2 + r1 + rl0, 3);
			}
			else
			{
				q_data[0] = MathUtil::RightShift((l1 << 1) + l0 + r1, 2);
			}
		}
	}
}

void LoopFilter::DoFilterNormal(int strength, std::vector<uint8_t>& p_data, std::vector<uint8_t>& q_data, int alpha, int beta)
{

}

std::pair<int, int> LoopFilter::CalculateAlphaAndBeta(std::shared_ptr<Macroblock> mb_p, std::shared_ptr<Macroblock> mb_q)
{
	int average_qp = MathUtil::RightShift(mb_p->GetQP() + mb_q->GetQP(), 1);

	int index_a = MathUtil::Clamp<int>(CommonConstantValues::s_min_qp, CommonConstantValues::s_max_qp, average_qp);
	int index_b = MathUtil::Clamp<int>(CommonConstantValues::s_min_qp, CommonConstantValues::s_max_qp, average_qp);
	
	int alpha = FilterConstantValues::s_alpha_table[index_a];
	int beta = FilterConstantValues::s_beta_table[index_b];

	return std::make_pair(alpha, beta);
}

std::vector<uint8_t> LoopFilter::GetLeftDataByVerticalEdge(std::shared_ptr<Macroblock> mb, int edge, uint32_t y)
{
	auto mb_data = mb->GetReconstructedLumaBlockData();

	uint32_t start_x = (16 + (edge - 1) * 4) % 16;
	uint32_t finish_x = edge * 4;

	std::vector<uint8_t> ret(4);
	for (uint32_t x = start_x; x < finish_x; ++x)
		ret[x - start_x] = mb_data.GetElement(x, y);

	std::reverse(ret.begin(), ret.end());
	return ret;
}

void LoopFilter::SetLeftDataByVerticalEdge(std::shared_ptr<Macroblock> mb, int edge, uint32_t y, std::vector<uint8_t>& p_data)
{
	auto mb_data = mb->GetReconstructedLumaBlockData();

	uint32_t start_x = (16 + (edge - 1) * 4) % 16;
	uint32_t finish_x = edge * 4;
	
	std::reverse(p_data.begin(), p_data.end());
	for (uint32_t x = start_x; x < finish_x; ++x)
		mb_data.SetElement(x, y, p_data[x - start_x]);
	
	mb->SetReconstructedLumaBlockData(mb_data);
}

std::vector<uint8_t> LoopFilter::GetRightDataByVerticalEdge(std::shared_ptr<Macroblock> mb, int edge, uint32_t y)
{
	auto mb_data = mb->GetReconstructedLumaBlockData();

	uint32_t start_x = edge * 4;
	uint32_t finish_x = edge * 4 + 4;

	std::vector<uint8_t> ret(4);
	for (uint32_t x = start_x; x < finish_x; ++x)
		ret[x - start_x] = mb_data.GetElement(x, y);

	return ret;
}

void LoopFilter::SetRightDataByVerticalEdge(std::shared_ptr<Macroblock> mb, int edge, uint32_t y, const std::vector<uint8_t>& q_data)
{
	auto mb_data = mb->GetReconstructedLumaBlockData();

	uint32_t start_x = edge * 4;
	uint32_t finish_x = edge * 4 + 4;

	std::vector<uint8_t> ret(4);
	for (uint32_t x = start_x; x < finish_x; ++x)
		mb_data.SetElement(x, y, ret[x - start_x]);

	mb->SetReconstructedLumaBlockData(mb_data);
}

__codec_end

