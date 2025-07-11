#include <iostream>

#include "log.h"
#include "encoder_config.h"
#include "encoder_config_reader.h"
#include "encoder.h"
#include "parameter_set_container.h"
#include "ostream.h"
#include "stream_util.h"
#include "slice.h""

using namespace codec;

int main(int argc, char* argv[])
{
	auto encoder_config = EncoderConfigReader::Read("config.txt");
	Encoder encoder(encoder_config);
	encoder.PrepareContext();
	encoder.Encode();

	LOGINFO("codec end.");

	return 0;
}