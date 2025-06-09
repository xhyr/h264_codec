#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "global_defines.h"

__codec_begin

struct EncoderConfig;

struct EncoderConfigReader
{
	static std::shared_ptr<EncoderConfig> Read(const std::string& file_path);

private:
	static std::shared_ptr<EncoderConfig> ParseConfigMap(const std::unordered_map<std::string, std::string>& config_map);

};

__codec_end