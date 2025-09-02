#pragma once

#include <cstdint>
#include <memory>

#include "slice_type.h"
#include "mb_type.h"
#include "prediction_type.h"

__codec_begin

class Slice;
class BytesData;
struct CavlcDataSource;

class MBIntraHeaderBinaryer
{
public:
	explicit MBIntraHeaderBinaryer(std::shared_ptr<BytesData> bytes_data);
	~MBIntraHeaderBinaryer();

	void OutputMBType(MBType mb_type, uint8_t intra16_offset = 0);

	void OutputChromaPredMode(IntraChromaPredictionType prediction_type);

	void OutputCBP(uint8_t cbp);

	void OutputQPDelta(int qp_delta);

private:
	std::shared_ptr<BytesData> m_bytes_data;
	MBType m_mb_type;
};

__codec_end