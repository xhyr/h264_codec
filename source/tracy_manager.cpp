#include "tracy_manager.h"

#include <thread>

#define TRACY_ENABLE
#include <tracy/Tracy.hpp>

__codec_begin

TracyManager::TracyManager()
{
	tracy::GetProfiler();

	WaitForConnection();
}

TracyManager::~TracyManager()
{
	ShutDown();
}

void TracyManager::WaitForConnection(int max_retries, int delay_ms)
{
	for (int i = 0; i < max_retries; ++i) 
	{
		if (tracy::GetProfiler().IsConnected())
			break;

		std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
	}
}

void TracyManager::ShutDown()
{
}

__codec_end