#include "file_util.h"

#include <filesystem>

#include "log.h"

__codec_begin

bool FileUtil::IsFileExist(const std::string& file_path)
{
	return std::filesystem::exists(file_path);
}

bool FileUtil::DuplicateFile(const std::string& source_file_path, const std::string& target_file_path)
{
	std::filesystem::path source_path(source_file_path);
	std::filesystem::path target_path(target_file_path);
	try
	{
		std::filesystem::copy_file(source_path, target_path, std::filesystem::copy_options::overwrite_existing);
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		LOGERROR("copy file error : %s.", e.what());
		return false;
	}

	return true;
}

__codec_end