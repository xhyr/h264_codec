#include <iostream>

#include "log.h"
#include "encoder_config.h"
#include "encoder_config_reader.h"

using namespace codec;

int main(int argc, char* argv[])
{
	LOGINFO("codec begin.");

	auto encoder_config = EncoderConfigReader::Read("config.txt");

	LOGINFO("codec end.");

	return 0;
}