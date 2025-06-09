#include "logger.h"

#include "logger_impl.h"

__codec_begin

Logger& Logger::Instance()
{
	static Logger instance;
	return instance;
}

void Logger::Log(LoggerLevel level, const std::string& message)
{
	m_impl->Log(level, message);
}

void Logger::SetLevel(LoggerLevel level)
{
	m_impl->SetLevel(level);
}

LoggerLevel Logger::GetLevel() const
{
	return m_impl->GetLevel();
}

void Logger::AnchorTime()
{
	m_impl->AnchorTime();
}

int64_t Logger::GetElapsedSeconds() const
{
	return m_impl->GetElapsedSeconds();
}

Logger::Logger() :m_impl(new LoggerImpl)
{

}

Logger::~Logger()
{
	delete m_impl;
}

__codec_end
