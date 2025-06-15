#include "stream_util.h"

#include "file_ostream.h"

__codec_begin

std::shared_ptr<OStream> StreamUtil::CreateFileOStream(const std::string& file_path)
{
	std::shared_ptr<OStream> out_stream;
	out_stream.reset(new FileOStream(file_path));
	if (!out_stream->Open())
		out_stream.reset();
	return out_stream;
}

__codec_end
