#pragma once

#include <cstdint>
#include <atomic>
#include <condition_variable>
#include <deque>
#include <fstream>
#include <mutex>
#include <thread>
#include <utility>
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

	void Loop();

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
	
	std::deque<std::pair<LoggerLevel, std::string>> m_log_cache;
	std::condition_variable m_conditional;
	std::thread m_thread;
	std::atomic<bool> m_to_stop{ false };
};

__codec_end