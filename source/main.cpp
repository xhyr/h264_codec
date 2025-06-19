#include <iostream>

#include "log.h"
#include "encoder_config.h"
#include "encoder_config_reader.h"
#include "encoder.h"
#include "parameter_set_container.h"
#include "ostream.h"
#include "stream_util.h"

using namespace codec;

int main(int argc, char* argv[])
{
	char a[] = "test";
	std::string_view s(a);

	LOGINFO("codec begin.");

	auto encoder_config = EncoderConfigReader::Read("config.txt");
	ParameterSetContainer parameter_set_container;
	parameter_set_container.InitConfig(encoder_config);
	parameter_set_container.ConstructSPS();
	parameter_set_container.ConstructPPS();

	auto out_stream = StreamUtil::CreateFileOStream("test.264");
	parameter_set_container.Serial(out_stream);

	LOGINFO("codec end.");

	return 0;
}