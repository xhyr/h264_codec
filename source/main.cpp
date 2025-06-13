#include <iostream>

#include "log.h"
#include "encoder_config.h"
#include "encoder_config_reader.h"
#include "encoder.h"

using namespace codec;

int main(int argc, char* argv[])
{
	LOGINFO("codec begin.");

	auto encoder_config = EncoderConfigReader::Read("config.txt");
	Encoder encoder(encoder_config);
	encoder.PrepareContext();

	LOGINFO("codec end.");

	return 0;
}