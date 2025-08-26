#include "coding_util.h"

#include "bytes_data.h"
#include "common_constant_values.h"
#include "math_util.h"

__codec_begin

uint32_t CodingUtil::U_1(uint8_t value, std::shared_ptr<BytesData> bytes_data)
{
	bytes_data->PushBit(value);
	return 1;
}

uint32_t CodingUtil::U_V(uint32_t n, uint32_t value, std::shared_ptr<BytesData> bytes_data)
{
	for (int index = n - 1; index >= 0; --index)
	{
		uint32_t mask = 1 << index;
		bytes_data->PushBit((mask & value) ? 1 : 0);
	}
	return n;
}

uint32_t CodingUtil::UE_V(uint32_t value, std::shared_ptr<BytesData> bytes_data)
{
	++value;
	uint32_t prefix_length = GetBinaryLength(value) - 1;
	bytes_data->PushBit(0, prefix_length);
	U_V(prefix_length + 1, value, bytes_data);
	return prefix_length * 2 + 1;
}

uint32_t CodingUtil::SE_V(int value, std::shared_ptr<BytesData> bytes_data)
{
	value = value <= 0 ? (-2 * value) : (2 * value - 1);
	return UE_V(value, bytes_data);
}

uint32_t CodingUtil::GetBinaryLength(uint32_t value)
{
	uint32_t length = value > 0 ? 1 : 0;
	while (value >>= 1)
		++length;
	return length;
}

std::shared_ptr<BytesData> CodingUtil::SODB2EBSP(std::shared_ptr<BytesData> bytes_data)
{
	SODB2RBSP(bytes_data);
	RBSP2EBSP(bytes_data);
	return bytes_data;
}

int CodingUtil::GetUEBitCount(uint32_t value)
{
	int prefix_bit_count = MathUtil::CeilLog2(value, 1) - 1;
	return prefix_bit_count * 2 + 1;
}

int CodingUtil::GetSEBitCount(int value)
{
	value = value <= 0 ? (-2 * value) : (2 * value - 1);
	return GetUEBitCount(value);
}

void CodingUtil::SODB2RBSP(std::shared_ptr<BytesData> bytes_data)
{
	bytes_data->PushBit(1);
	bytes_data->FillLastByte(0);
}

void CodingUtil::RBSP2EBSP(std::shared_ptr<BytesData> bytes_data)
{
	auto bytes_count = bytes_data->GetBytesCount();
	BytesData new_bytes_data;
	new_bytes_data.Reserve(bytes_count);

	uint32_t index = 0;
	uint32_t zero_count = 0;
	while (index < bytes_count)
	{
		auto value = bytes_data->GetByte(index++);
		if (zero_count == CommonConstantValues::s_zero_bytes_short_start_code && !(value & 0xFC))
		{
			new_bytes_data.PushByte(3);
			zero_count = 0;
		}
		new_bytes_data.PushByte(value);
		zero_count = value == 0 ? (zero_count + 1) : 0;
	}

	*bytes_data = new_bytes_data;
}


__codec_end
