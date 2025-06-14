#pragma once

#include <cstdint>
#include <memory>

#include "global_defines.h"

__codec_begin

class BytesData;

struct CodingUtil
{
	static uint32_t U_1(uint8_t value, std::shared_ptr<BytesData> bytes_data);

	static uint32_t U_V(uint8_t n, uint8_t value, std::shared_ptr<BytesData> bytes_data);

	static uint32_t UE_V(uint32_t value, std::shared_ptr<BytesData> bytes_data);

	static uint32_t SE_V(int value, std::shared_ptr<BytesData> bytes_data);

	static uint32_t GetBinaryLength(uint32_t value);

	static void SODB2EBSP(std::shared_ptr<BytesData> bytes_data);

	static void SODB2RBSP(std::shared_ptr<BytesData> bytes_data);

	static void RBSP2EBSP(std::shared_ptr<BytesData> bytes_data);

};

__codec_end