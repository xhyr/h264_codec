#pragma once

#include <cstdint>
#include <vector>

#include "global_defines.h"

__codec_begin

template<size_t Width, size_t Height, typename Ty = uint8_t>
class BlockData
{
public:
	void SetData(const std::vector<uint8_t>& in_data)
	{
		m_data = in_data;
	}

	void SetElement(uint32_t x, uint32_t y, Ty value)
	{
		m_data[y * Width + x] = value;
	}

	void SetElementInRow(uint32_t y, const std::vector<Ty>& values)
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

	std::vector<uint8_t> GetRightData() const
	{
		std::vector<Ty> right_data(Height);
		for (uint32_t y = 0; y < Height; ++y)
			right_data[y] = m_data[y * Width + Width - 1];
		return right_data;
	}

	std::vector<uint8_t> GetDownData() const
	{
		std::vector<Ty> down_data(&m_data[(Height - 1) *Width], &m_data[Height * Width]);
		return down_data;
	}

private:
	std::vector<Ty> m_data;

};

__codec_end