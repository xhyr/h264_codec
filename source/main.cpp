#include <iostream>

#include "log.h"
#include "encoder_config.h"
#include "encoder_config_reader.h"
#include "encoder.h"
#include "parameter_set_mgr.h"
#include "ostream.h"
#include "stream_util.h"

using namespace codec;

int main(int argc, char* argv[])
{
	LOGINFO("codec begin.");

	auto encoder_config = EncoderConfigReader::Read("config.txt");
	ParameterSetMgr parameter_set_mgr;
	parameter_set_mgr.InitConfig(encoder_config);
	parameter_set_mgr.ConstructSPS();

	auto out_stream = StreamUtil::CreateFileOStream("test.264");
	parameter_set_mgr.SerialSPS(out_stream);

	LOGINFO("codec end.");

	return 0;
}