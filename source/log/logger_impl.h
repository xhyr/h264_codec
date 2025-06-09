#pragma once

#include <cstdint>
#include <fstream>
#include <mutex>
#include <vector>
#include <Windows.h>

#include "logger_level.h"

__codec_begin

class LoggerImpl
{
public:
	LoggerImpl();
	~LoggerImpl();

	void Log(LoggerLevel level, const std::string& message);

	void SetLevel(LoggerLevel level);

	LoggerLevel GetLevel() const;

	void AnchorTime();

	int64_t GetElapsedSeconds() const;

private:
	void Init();

	void BackUp();

	static std::string GetTimeDescription();

private:
	mutable std::mutex m_mutex;
	LoggerLevel m_level;
	std::ofstream m_stream;

	int64_t m_frequency;
	LARGE_INTEGER m_anchor_time;
};

__codec_end