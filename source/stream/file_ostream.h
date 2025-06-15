#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <fstream>

#include "global_defines.h"

#include "ostream.h"

__codec_begin

class BytesData;

class FileOStream : public OStream
{
public:
	explicit FileOStream(const std::string& file_path);
	virtual ~FileOStream();

	virtual bool Open() override;

	virtual void PushBytesData(std::shared_ptr<BytesData> bytes_data) override;

	virtual void Flush() override;

	virtual void Close() override;

private:
	std::string m_file_path;
	std::ofstream m_handle;
};

__codec_end