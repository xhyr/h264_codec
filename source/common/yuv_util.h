#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "global_defines.h"

__codec_begin

struct YUVUtil
{
	static std::tuple< std::shared_ptr<uint8_t[]>, std::shared_ptr<uint8_t[]>, std::shared_ptr<uint8_t[]>> Convert2YUV444P(uint32_t width, uint32_t height, uint8_t channel_num, const std::vector<uint8_t>& data);
	
	static std::tuple< std::shared_ptr<uint8_t[]>, std::shared_ptr<uint8_t[]>, std::shared_ptr<uint8_t[]>> Convert2YUV420P(uint32_t width, uint32_t height, uint8_t channel_num, const std::vector<uint8_t>& data);

	static bool Serial(uint32_t width, uint32_t height, const std::string& file_path, std::shared_ptr<uint8_t[]> y_data, std::shared_ptr<uint8_t[]> u_data, std::shared_ptr<uint8_t[]> v_data);

	static bool Unserial(uint32_t width, uint32_t height, const std::string& file_path, std::shared_ptr<uint8_t[]>& y_data, std::shared_ptr<uint8_t[]>& u_data, std::shared_ptr<uint8_t[]>& v_data, uint32_t tick);

};

__codec_end