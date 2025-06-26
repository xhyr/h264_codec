#include "png_predictor.h"

__codec_begin

PngPredictor::PngPredictor(uint32_t width, uint32_t height, std::shared_ptr<uint8_t[]> data) :
	m_width(width), m_height(height), m_data(data)
{
}

PngPredictor::~PngPredictor()
{
}

int PngPredictor::Predict()
{
	int cost = 0;
	/*for (uint32_t y = 0; y < m_height; ++y)
	{
		auto min_cost = PredictOneRow(y);
		cost += min_cost;
	}*/

	int block_width = 4;
	int block_height = 4;
	for (uint32_t y = 0; y < m_height; y += block_height)
	{
		for (uint32_t x = 0; x < m_width; x += block_width)
		{
			auto min_cost = PredictOneBlock(x, x + block_width, y, y + block_height);
			cost += min_cost;
		}
	}

	return cost;
}

int PngPredictor::PredictOneRow(uint32_t y) const
{
	int min_cost = 0;
	int cost = 0;
	for (uint32_t x = 0; x < m_width; ++x)
	{
		uint8_t value = m_data.get()[y * m_width + x];
		auto tmp_cost = GetNoneCost(value);
		cost += tmp_cost;
	}
	min_cost = cost;

	cost = 0;
	for (uint32_t x = 0; x < m_width; ++x)
	{
		auto left = GetLeft(x, y);
		uint8_t value = m_data.get()[y * m_width + x];
		auto tmp_cost = GetSubCost(value, left);
		cost += tmp_cost;
	}
	if (cost < min_cost) min_cost = cost;

	cost = 0;
	for (uint32_t x = 0; x < m_width; ++x)
	{
		auto up = GetUp(x, y);
		uint8_t value = m_data.get()[y * m_width + x];
		auto tmp_cost = GetUpCost(value, up);
		cost += tmp_cost;
	}
	if (cost < min_cost) min_cost = cost;

	cost = 0;
	for (uint32_t x = 0; x < m_width; ++x)
	{
		auto left = GetLeft(x, y);
		auto up = GetUp(x, y);
		uint8_t value = m_data.get()[y * m_width + x];
		auto tmp_cost = GetAverageCost(value, left, up);
		cost += tmp_cost;
	}
	if (cost < min_cost) min_cost = cost;

	cost = 0;
	for (uint32_t x = 0; x < m_width; ++x)
	{
		auto left = GetLeft(x, y);
		auto up = GetUp(x, y);
		auto left_up = GetLeftUp(x, y);
		uint8_t value = m_data.get()[y * m_width + x];
		auto tmp_cost = GetPaethCost(value, left, up, left_up);
		cost += tmp_cost;
	}
	if (cost < min_cost) min_cost = cost;

	return min_cost;
}

int PngPredictor::PredictOneBlock(uint32_t begin_x, uint32_t end_x, uint32_t begin_y, uint32_t end_y) const
{
	int min_cost = 0;
	int cost = 0;
	for (uint32_t y = begin_y; y < end_y; ++y)
	{
		for (uint32_t x = begin_x; x < end_x; ++x)
		{
			uint8_t value = m_data.get()[y * m_width + x];
			auto tmp_cost = GetNoneCost(value);
			cost += tmp_cost;
		}
	}
	min_cost = cost;

	cost = 0;
	for (uint32_t y = begin_y; y < end_y; ++y)
	{
		for (uint32_t x = begin_x; x < end_x; ++x)
		{
			auto left = GetLeft(x, y);
			uint8_t value = m_data.get()[y * m_width + x];
			auto tmp_cost = GetSubCost(value, left);
			cost += tmp_cost;
		}
	}
	if (cost < min_cost) min_cost = cost;

	cost = 0;
	for (uint32_t y = begin_y; y < end_y; ++y)
	{
		for (uint32_t x = begin_x; x < end_x; ++x)
		{
			auto up = GetUp(x, y);
			uint8_t value = m_data.get()[y * m_width + x];
			auto tmp_cost = GetUpCost(value, up);
			cost += tmp_cost;
		}
	}
	if (cost < min_cost) min_cost = cost;

	cost = 0;
	for (uint32_t y = begin_y; y < end_y; ++y)
	{
		for (uint32_t x = begin_x; x < end_x; ++x)
		{
			auto left = GetLeft(x, y);
			auto up = GetUp(x, y);
			uint8_t value = m_data.get()[y * m_width + x];
			auto tmp_cost = GetAverageCost(value, left, up);
			cost += tmp_cost;
		}
	}
	if (cost < min_cost) min_cost = cost;

	cost = 0;
	for (uint32_t y = begin_y; y < end_y; ++y)
	{
		for (uint32_t x = begin_x; x < end_x; ++x)
		{
			auto left = GetLeft(x, y);
			auto up = GetUp(x, y);
			auto left_up = GetLeftUp(x, y);
			uint8_t value = m_data.get()[y * m_width + x];
			auto tmp_cost = GetPaethCost(value, left, up, left_up);
			cost += tmp_cost;
		}
	}
	if (cost < min_cost) min_cost = cost;

	return min_cost;
}

int PngPredictor::GetLeft(uint32_t x, uint32_t y) const
{
	return x == 0 ? 0 : m_data.get()[y * m_width + x - 1];
}

int PngPredictor::GetUp(uint32_t x, uint32_t y) const
{
	return y == 0 ? 0 : m_data.get()[(y - 1) * m_width + x];
}

int PngPredictor::GetLeftUp(uint32_t x, uint32_t y) const
{
	return x == 0 || y == 0 ? 0 : m_data.get()[(y - 1) * m_width + x - 1];
}

int PngPredictor::GetNoneCost(int value) const
{
	return value;
}

int PngPredictor::GetSubCost(int value, int left) const
{
	return abs(value - left);
}

int PngPredictor::GetUpCost(int value, int up) const
{
	return abs(value - up);
}

int PngPredictor::GetAverageCost(int value, int left, int up) const
{
	return abs(value - (left + up) / 2);
}

int PngPredictor::GetPaethCost(int value, int left, int up, int left_up) const
{
	int p = left + up - left_up;
	int pa = abs(p - left);
	int pb = abs(p - up);
	int pc = abs(p - left_up);
	if (pa <= pb && pa <= pc) return abs(value - left);
	else if (pb <= pc) return abs(value - up);
	else return abs(value - left_up);
}

__codec_end

