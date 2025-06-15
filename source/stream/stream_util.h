#pragma once

#include <memory>
#include <string>

#include "global_defines.h"

__codec_begin

class OStream;

struct StreamUtil
{
	static std::shared_ptr<OStream> CreateFileOStream(const std::string& file_path);


};

__codec_end