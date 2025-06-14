#include "file_util.h"

#include <filesystem>
#include <fstream>

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

bool FileUtil::ReadFile2Lines(const std::string& file_path, std::vector<std::string>& lines)
{
	std::ifstream in_file(file_path);
	if (!in_file.is_open())
		return false;

	std::string line;
	while (std::getline(in_file, line))
		lines.push_back(line);

	return true;
}

bool FileUtil::WriteToFile(const std::vector<uint8_t>& data, const std::string& file_path)
{
	if (data.empty())
		return true;

	std::ofstream out_file(file_path, std::ios::binary | std::ios::out);
	if (!out_file)
		return false;

	out_file.write(reinterpret_cast<const char*>(data.data()), data.size());
	return out_file.tellp() == data.size();
}

bool FileUtil::WriteToFile(const uint8_t* data_ptr, uint32_t data_size, const std::string& file_path)
{
	if (data_size == 0)
		return true;

	std::ofstream out_file(file_path, std::ios::binary | std::ios::out);
	if (!out_file)
		return false;

	out_file.write(reinterpret_cast<const char*>(data_ptr), data_size);
	return out_file.tellp() == data_size;
}

__codec_end