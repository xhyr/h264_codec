#pragma once

#include <cstdint>
#include <memory>

#include "slice_type.h"
#include "mb_type.h"

__codec_begin

class Slice;
class BytesData;

class MBInterHeaderBinaryer
{
public:
	explicit MBInterHeaderBinaryer(std::shared_ptr<BytesData> bytes_data);
	~MBInterHeaderBinaryer();

	void OutputMBSkipRun(uint32_t skip_run);

	void OutputMBType(MBType mb_type);

	void OutputCBP(uint8_t cbp);

	void OutputQPDelta(int qp_delta);

private:
	std::shared_ptr<BytesData> m_bytes_data;
};

__codec_end