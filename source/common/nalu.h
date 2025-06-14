#pragma once

#include <memory>

#include "global_defines.h"

__codec_begin

class BytesData;

class Nalu
{
public:
	explicit Nalu();
	~Nalu();



private:
	std::shared_ptr<BytesData> m_data;

};

__codec_end