#pragma once

#include <cstdint>
#include <memory>

#include "global_defines.h"

__codec_begin

class BytesData;

class OStream
{
public:
	explicit OStream();
	virtual ~OStream();

	virtual bool Open() = 0;

	virtual void PushBytesData(std::shared_ptr<BytesData> bytes_data) = 0;

	virtual void Flush() = 0;

	virtual void Close() = 0;
};

__codec_end