#pragma once

#include <cstdint>
#include <string>

#include "logger_level.h"

__codec_begin

class LoggerImpl;

class Logger
{
public:
	static Logger& Instance();

	void Log(LoggerLevel level, const std::string& message);

	void SetLevel(LoggerLevel level);

	LoggerLevel GetLevel() const;

	void AnchorTime();

	int64_t GetElapsedSeconds() const;

private:
	Logger();
	~Logger();

	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

private:
	LoggerImpl* m_impl;
};

__codec_end

