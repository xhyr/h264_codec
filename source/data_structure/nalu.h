#pragma once

#include <memory>

#include "codec_types.h"

__codec_begin

class BytesData;
class OStream;

class Nalu
{
public:
	explicit Nalu(NaluType nalu_type);
	~Nalu();

	void SetData(std::shared_ptr<BytesData> bytes_data);

	void Serial(std::shared_ptr<OStream> ostream);

private:
	NaluType m_type;
	std::shared_ptr<BytesData> m_data;
};

__codec_end