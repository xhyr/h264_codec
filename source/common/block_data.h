#pragma once

#include <cstdint>
#include <memory>
#include <numeric>
#include <vector>

#include "position.h"

__codec_begin

template<size_t Width, size_t Height, typename Ty = uint8_t>
class BlockData
{
public:
	BlockData() : m_data(Width* Height, 0)
	{

	}

	void SetData(const std::vector<Ty>& in_data)
	{
		m_data = in_data;
	}

	std::vector<Ty> GetData() const
	{
		return m_data;
	}

	void SetElement(uint32_t x, uint32_t y, Ty value)
	{
		m_data[y * Width + x] = value;
	}

	Ty GetElement(uint32_t x, uint32_t y) const
	{
		return m_data[y * Width + x];
	}

	Ty GetElement(Position position) const
	{
		return m_data[position.y * Width + position.x];
	}

	Ty GetElement(uint32_t index) const
	{
		return m_data[index];
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
		std::vector<Ty> down_data(std::next(m_data.begin(), (Height - 1) * Width), m_data.end());
		return down_data;
	}

	BlockData<4, 4, int32_t> GetBlock4x4(uint32_t x_in_block, uint32_t y_in_block) const
	{
		BlockData<4, 4, int32_t> result;
		for (uint32_t y = 4 * y_in_block; y < 4 * y_in_block + 4; ++y)
			for (uint32_t x = 4 * x_in_block; x < 4 * x_in_block + 4; ++x)
				result.SetElement(x - 4 * x_in_block, y - 4 * y_in_block, GetElement(x, y));
		return result;
	}

	int GetSum() const
	{
		return std::accumulate(m_data.begin(), m_data.end(), 0);
	}

	int GetAbstractSum() const
	{
		return std::reduce(m_data.begin(), m_data.end(), 0, [](int sum, Ty value) {return sum + std::abs(value); });
	}

	void Reset(Ty value)
	{
		std::fill(m_data.begin(), m_data.end(), value);
	}

	std::shared_ptr<Ty[]> Convert2DataPtr() const
	{
		std::shared_ptr<Ty[]> data_ptr(new Ty[Width * Height]);
		std::copy(m_data.begin(), m_data.end(), data_ptr.get());
		return data_ptr;
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
	return output;
}

__codec_end