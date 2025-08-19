#include "bytes_data.h"

__codec_begin

void BytesData::PushByte(uint8_t value, uint32_t continous_count)
{
	for (uint32_t i = 0; i < continous_count; ++i)
	{
		m_data.push_back(value);
		m_bits_count += 8;
	}
}

uint8_t BytesData::GetByte(uint32_t index) const
{
	return m_data[index];
}

void BytesData::PushBit(uint8_t value, uint32_t continous_count)
{
	for (uint32_t i = 0; i < continous_count; ++i)
	{
		if (IsByteAlign())
			m_data.push_back(0);

		auto& byte_value = m_data.back();
		uint32_t bit_offset = 7 - m_bits_count % 8;
		byte_value |= value << bit_offset;
		++m_bits_count;
	}
}

uint8_t BytesData::GetBit(uint32_t index) const
{
	auto byte_value = m_data[index / 8];
	uint32_t bit_offset = 7 - index % 8;
	return (byte_value >> bit_offset) & 0x01;
}

uint32_t BytesData::GetBytesCount() const
{
	return m_data.size();
}

uint32_t BytesData::GetBitsCount() const
{
	return m_bits_count;
}

std::vector<uint8_t> BytesData::GetData() const
{
	return m_data;
}

const uint8_t* BytesData::GetDataPtr() const
{
	return m_data.data();
}

void BytesData::SetData(const std::vector<uint8_t>& data)
{
	m_data = data;
}

bool BytesData::IsByteAlign() const
{
	return m_bits_count % 8 == 0;
}

void BytesData::BytesData::FillLastByte(uint8_t value)
{
	while (m_bits_count % 8 != 0)
		PushBit(value);
}

void BytesData::Reserve(uint32_t bits_count)
{
	m_data.reserve((bits_count + 7) / 8);
}

void BytesData::Push(std::shared_ptr<BytesData> bytes_data)
{
	auto bit_count = bytes_data->GetBitsCount();
	if (IsByteAlign())
	{
		auto byte_count = bit_count / 8;
		for (uint32_t i = 0; i < byte_count; ++i)
			PushByte(bytes_data->GetByte(i));
		for (uint32_t i = byte_count * 8; i < bit_count; ++i)
			PushBit(bytes_data->GetBit(i));
	}
	else
	{
		for (uint32_t i = 0; i < bit_count; ++i)
			PushBit(bytes_data->GetBit(i));
	}
}

__codec_end