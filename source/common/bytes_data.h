#pragma once

#include <cstdint>
#include <vector>

#include "global_defines.h"

__codec_begin

class BytesData
{
public:
	void PushByte(uint8_t value, uint32_t continous_count = 1);

	uint8_t GetByte(uint32_t index) const;

	void PushBit(uint8_t value, uint32_t continous_count = 1);

	uint8_t GetBit(uint32_t index) const;

	uint32_t GetBytesCount() const;

	uint32_t GetBitsCount() const;

	std::vector<uint8_t> GetData() const;

	const uint8_t* GetDataPtr() const;

	void SetData(const std::vector<uint8_t>& data);

	bool IsByteAlign() const;

	void FillLastByte(uint8_t value);

	void Reserve(uint32_t bits_count);

private:
	std::vector<uint8_t> m_data;
	uint32_t m_bits_count{ 0 };
};

__codec_end