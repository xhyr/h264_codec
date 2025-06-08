#include "file_util.h"

#include <filesystem>

__codec_begin

bool FileUtil::IsFileExist(const std::string& file_path)
{
	return std::filesystem::exists(file_path);
}

__codec_end