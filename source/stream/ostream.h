#pragma once

#include <cstdint>
#include <string>

#include "global_defines.h"

__codec_begin

class OStream
{
public:
	explicit OStream(const std::string& file_path);
	virtual ~OStream();



};

__codec_end