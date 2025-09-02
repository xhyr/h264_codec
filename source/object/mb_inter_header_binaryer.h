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

	void WriteReferenceFrame();

	void WriteMotionVector();

	void OutputCBP(uint8_t cbp);

	void OutputQPDelta(int qp_delta);

private:
	std::weak_ptr<Slice> m_slice;
	uint32_t m_mb_addr;
	std::shared_ptr<BytesData> m_bytes_data;
	SliceType m_slice_type;
	MBType m_mb_type;
	uint8_t m_cbp;
};

__codec_end