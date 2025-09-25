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

		iter = config_map.find("start_frame");
		if (iter == config_map.end())
			break;
		config->start_frame = std::stoi(iter->second);

		iter = config_map.find("frames_to_encode");
		if (iter == config_map.end())
			break;
		config->frames_to_encode = std::stoi(iter->second);

		iter = config_map.find("profile_idc");
		if (iter == config_map.end())
			break;
		config->profile_idc = std::stoi(iter->second);

		iter = config_map.find("level_idc");
		if (iter == config_map.end())
			break;
		config->level_idc = std::stoi(iter->second);

		iter = config_map.find("ref_frame_number");
		if (iter == config_map.end())
			break;
		config->ref_frame_number = std::stoi(iter->second);

		iter = config_map.find("qp");
		if (iter == config_map.end())
			break;
		config->qp = std::stoi(iter->second);

		iter = config_map.find("search_range");
		if (iter == config_map.end())
			break;
		config->search_range = std::stoi(iter->second);

		return config;
	} while (0);


	return nullptr;
}

__codec_end

