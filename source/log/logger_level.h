#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

enum class LoggerLevel : uint8_t
{
	Error = 0,
	Warning,
	Info,
	Debug
};

__codec_end