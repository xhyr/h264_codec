#pragma once

#include "global_defines.h"

__codec_begin

class TracyManager
{
public:
	TracyManager();

	~TracyManager();

private:
	void WaitForConnection(int max_retries = 50, int delay_ms = 100);

	void ShutDown();
};

__codec_end