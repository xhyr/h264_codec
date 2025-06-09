#pragma once

#include <string>
#include <vector>

#include "global_defines.h"

__codec_begin

struct FileUtil
{
	static bool IsFileExist(const std::string& file_path);

	static bool DuplicateFile(const std::string& source_file_path, const std::string& target_file_path);

	static bool ReadFile2Lines(const std::string& file_path, std::vector<std::string>& lines);

};

__codec_end