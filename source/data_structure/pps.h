#pragma once

#include <memory>

#include "global_defines.h"

__codec_begin

struct PPSData;
class BytesData;

class PPS
{
public:
	PPS();
	~PPS();

	std::shared_ptr<PPSData> GetData();

	std::shared_ptr<BytesData> Encapsulate();

private:
	std::shared_ptr<PPSData> m_data;
};


__codec_end