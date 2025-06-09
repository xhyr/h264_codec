#pragma once

#include <string>

#include "global_defines.h"

__codec_begin

struct FileUtil
{
	static bool IsFileExist(const std::string& file_path);

	static bool DuplicateFile(const std::string& source_file_path, const std::string& target_file_path);

};

__codec_end