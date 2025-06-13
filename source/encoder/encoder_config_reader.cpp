#include "encoder_config_reader.h"

#include "encoder_config.h"
#include "config_util.h"

__codec_begin

std::shared_ptr<EncoderConfig> EncoderConfigReader::Read(const std::string& file_path)
{
	do
	{
		std::unordered_map<std::string, std::string> config_map;
		if (!ConfigUtil::ParseConfig(file_path, config_map))
			break;

		auto config = ParseConfigMap(config_map);

		return config;
	} while (0);


	return nullptr;
}

std::shared_ptr<EncoderConfig> EncoderConfigReader::ParseConfigMap(const std::unordered_map<std::string, std::string>& config_map)
{
	std::shared_ptr<EncoderConfig> config = std::make_shared<EncoderConfig>();
	do
	{
		auto iter = config_map.find("input_file_path");
		if (iter == config_map.end())
			break;
		config->input_file_path = iter->second;

		iter = config_map.find("output_file_path");
		if (iter == config_map.end())
			break;
		config->output_file_path = iter->second;

		iter = config_map.find("width");
		if (iter == config_map.end())
			break;
		config->width = std::stoi(iter->second);

		iter = config_map.find("height");
		if (iter == config_map.end())
			break;
		config->height = std::stoi(iter->second);

		return config;
	} while (0);


	return nullptr;
}

__codec_end

