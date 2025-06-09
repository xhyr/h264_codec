#include "config_util.h"

#include <regex>

#include "file_util.h"
#include "log.h"

__codec_begin

bool ConfigUtil::ParseConfig(const std::string& file_path, std::unordered_map<std::string, std::string>& config_map)
{
	do
	{
		std::vector<std::string> lines;
		bool got_file = FileUtil::ReadFile2Lines(file_path, lines);
		if (!got_file)
			break;

		ParseLines2Map(lines, config_map);

		return true;
	} while (0);

	return false;
}

void ConfigUtil::ParseLines2Map(const std::vector<std::string>& lines, std::unordered_map<std::string, std::string>& config_map)
{
	for (const auto& line : lines)
	{
		if (line[0] == '/' && line[1] == '/')
			continue;

		std::string name, value;
		bool ret = ParseLine(line, name, value);
		if (ret)
			config_map[name] = value;
	}
}

bool ConfigUtil::ParseLine(const std::string& line, std::string& name, std::string& value)
{
	std::regex pattern("\\s*([^\\s=]+)\\s*=\\s*([^\\s=]+)\\s*");

	std::smatch match;
	if (!std::regex_search(line, match, pattern))
		return false;

	name = match[1].str();
	value = match[2].str();

	return true;
}

__codec_end


