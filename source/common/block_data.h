#pragma once

#include <cstdint>
#include <numeric>
#include <vector>

#include "global_defines.h"

__codec_begin

template<size_t Width, size_t Height, typename Ty = uint8_t>
class BlockData
{
public:
	BlockData() : m_data(Width* Height, 0)
	{

	}

	void SetData(const std::vector<uint8_t>& in_data)
	{
		m_data = in_data;
	}

	void SetElement(uint32_t x, uint32_t y, Ty value)
	{
		m_data[y * Width + x] = value;
	}

	Ty GetElement(uint32_t x, uint32_t y) const
	{
		return m_data[y * Width + x];
	}

	void SetElementInRow(uint32_t y, const std::vector<uint8_t>& values)
	{
		std::copy(values.begin(), values.end(), std::next(m_data.begin(), y * Width));
	}

	void SetElementInColumn(uint32_t x, const std::vector<Ty>& values)
	{
		for (uint32_t y = 0; y < Height; ++y)
			m_data[y * Width + x] = values[y];
	}

	void SetElementAll(Ty value)
	{
		std::fill(m_data.begin(), m_data.end(), value);
	}

	std::vector<Ty> GetRightData() const
	{
		std::vector<Ty> right_data(Height);
		for (uint32_t y = 0; y < Height; ++y)
			right_data[y] = m_data[y * Width + Width - 1];
		return right_data;
	}

	std::vector<Ty> GetDownData() const
	{
		std::vector<Ty> down_data(&m_data[(Height - 1) *Width], &m_data[Height * Width]);
		return down_data;
	}

	int GetSum() const
	{
		return std::accumulate(m_data.begin(), m_data.end(), 0);
	}

	int GetAbstractSum() const
	{
		return std::reduce(m_data.begin(), m_data.end(), 0, [](int sum, Ty value) {return sum + std::abs(value); });
	}

private:
	std::vector<Ty> m_data;
};

template<size_t Width, size_t Height, typename Ty>
BlockData<Width, Height, int> operator-(const BlockData<Width, Height, Ty>& left_block_data, const BlockData<Width, Height, Ty>& right_block_data)
{
	BlockData<Width, Height, int> output;
	for (uint32_t y = 0; y < Height; ++y)
	{
		for (uint32_t x = 0; x < Width; ++x)
		{
			int left_value = left_block_data.GetElement(x, y);
			int right_value = right_block_data.GetElement(x, y);
			output.SetElement(x, y, left_value - right_value);
		}
	}
}

__codec_end