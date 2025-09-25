#include <chrono>
#include <iostream>

#include "log.h"
#include "encoder_config.h"
#include "encoder_config_reader.h"
#include "encoder.h"

using namespace codec;

int main(int argc, char* argv[])
{
	auto start_time = std::chrono::steady_clock::now();

	auto encoder_config = EncoderConfigReader::Read("config.txt");
	Encoder encoder(encoder_config);
	encoder.PrepareContext();
	encoder.Encode();

	auto finish_time = std::chrono::steady_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish_time - start_time).count();

	LOGINFO("codec end. use %lld ms.", ms);

	return 0;
}