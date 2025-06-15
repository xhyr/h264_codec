#include "file_ostream.h"

#include "bytes_data.h"

__codec_begin

FileOStream::FileOStream(const std::string& file_path) : m_file_path(file_path)
{
	
}

FileOStream::~FileOStream()
{
}

bool FileOStream::Open()
{
	m_handle.open(m_file_path, std::ios::binary | std::ios::out);
	return !!m_handle;
}

void FileOStream::PushBytesData(std::shared_ptr<BytesData> bytes_data)
{
	m_handle.write(reinterpret_cast<const char*>(bytes_data->GetDataPtr()), bytes_data->GetBytesCount());
}

void FileOStream::Flush()
{
	m_handle.flush();
}

void FileOStream::Close()
{
	m_handle.close();
}

__codec_end