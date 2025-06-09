#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include "global_defines.h"

__codec_begin

struct ConfigUtil
{
	static bool ParseConfig(const std::string& file_path, std::unordered_map<std::string, std::string>& config_map);

private:
	static void ParseLines2Map(const std::vector<std::string>& lines, std::unordered_map<std::string, std::string>& config_map);

	static bool ParseLine(const std::string& line, std::string& name, std::string& value);

};

__codec_end