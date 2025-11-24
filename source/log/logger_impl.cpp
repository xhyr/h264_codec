#include "logger_impl.h"

#include <cstdio>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "file_util.h"

__codec_begin

const static uint32_t s_max_buffer_size = 2048;

LoggerImpl::LoggerImpl() :m_level(LoggerLevel::Debug)
{
	Init();
}

LoggerImpl::~LoggerImpl()
{
	m_to_stop = true;
	m_conditional.notify_one();
	m_thread.join();

	BackUp();
}

void LoggerImpl::Log(LoggerLevel level, const std::string& message)
{
	std::string out_message = GetTimeDescription();
	out_message += " : ";
	out_message += message;
	out_message += "\n";

	std::lock_guard<std::mutex> lk(m_mutex);
	if (level > m_level)
		return;
	m_log_cache.emplace_back(level, out_message);
	m_conditional.notify_one();
}

void LoggerImpl::SetLevel(LoggerLevel level)
{
	std::lock_guard<std::mutex> lk(m_mutex);
	m_level = level;
}

LoggerLevel LoggerImpl::GetLevel() const
{
	std::lock_guard<std::mutex> lk(m_mutex);
	return m_level;
}

void LoggerImpl::AnchorTime()
{
	QueryPerformanceCounter(&m_anchor_time);
}

int64_t LoggerImpl::GetElapsedSeconds() const
{
	LARGE_INTEGER now_time;
	QueryPerformanceCounter(&now_time);
	auto elapsed_time = now_time.QuadPart - m_anchor_time.QuadPart;
	return elapsed_time / m_frequency;
}

void LoggerImpl::Loop()
{
	while (!m_to_stop)
	{
		LoggerLevel level;
		std::string message;

		{
			std::unique_lock<std::mutex> lk(m_mutex);
			m_conditional.wait(lk, [this]() {return m_to_stop || !m_log_cache.empty(); });
			std::tie(level, message) = m_log_cache.front();
			m_log_cache.pop_front();
		}

		m_stream << message;
		m_stream.flush();

		if (level <= LoggerLevel::Info)
			printf("%s", message.c_str());
	}
}

void LoggerImpl::Init()
{
	m_stream.open("log.log");

	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	m_frequency = frequency.QuadPart;

	m_thread = std::thread([this]() {Loop(); });
}

void LoggerImpl::BackUp()
{
	m_stream.flush();
	m_stream.close();

	std::string back_up_file_name = GetTimeDescription() + "_log.log";
	FileUtil::DuplicateFile("log.log", back_up_file_name);
}

std::string LoggerImpl::GetTimeDescription()
{
	auto now = std::chrono::system_clock::now();
	auto time_t_now = std::chrono::system_clock::to_time_t(now);
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
	std::tm tm_now = *std::localtime(&time_t_now);
	std::ostringstream oss;
	oss << std::put_time(&tm_now, "%Y_%m_%d_%H_%M_%S") << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();
	return oss.str();
}

__codec_end