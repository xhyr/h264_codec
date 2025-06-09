#pragma once

#include "logger_level.h"

__codec_begin

void Log(LoggerLevel level, const char* format, ...);

#define LOGERROR(...) (Log(LoggerLevel::Error, __VA_ARGS__))
#define LOGWARNING(...) (Log(LoggerLevel::Warning, __VA_ARGS__))
#define LOGINFO(...) (Log(LoggerLevel::Info, __VA_ARGS__))
#define LOGDEBUG(...) (Log(LoggerLevel::Debug, __VA_ARGS__))
#define LOG(level, ...) (Log(level, __VA_ARGS__))

__codec_end