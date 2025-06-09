#include "log.h"

#include <cstdarg>
#include <cstdint>
#include <string>
#include <vector>

#include "logger.h"

__codec_begin

const static uint32_t s_max_message_size = 2048;

void Log(LoggerLevel level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::vector<char> buffer(s_max_message_size, 0);
	vsnprintf(buffer.data(), s_max_message_size, format, args);
	va_end(args);

	std::string message(buffer.data());

	Logger::Instance().Log(level, message);
}

__codec_end