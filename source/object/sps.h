#pragma once

#include <memory>
#include <string>

#include "global_defines.h"

__codec_begin

struct SPSData;
class BytesData;

class SPS
{
public:
	SPS();
	~SPS();

	std::shared_ptr<SPSData> GetData();

	std::shared_ptr<BytesData> Encapsulate();

private:
	std::shared_ptr<SPSData> m_data;
};

__codec_end