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

class MBBinaryer
{
public:
	MBBinaryer(std::weak_ptr<Slice> slice, uint32_t mb_addr, std::shared_ptr<BytesData> bytes_data);
	~MBBinaryer();

	void OutputMBType(MBType mb_type, uint8_t intra16_offset = 0);

	void OutputChromaPredMode(IntraChromaPredictionType prediction_type);

	void OutputCBP(uint8_t cbp);

	void OutputQPDelta(int qp_delta);

	void OutputLumaCoeffs(const CavlcDataSource& data_source);

	void OutputChromaCoeffs(const CavlcDataSource& data_source);

private:
	std::weak_ptr<Slice> m_slice;
	uint32_t m_mb_addr;
	std::shared_ptr<BytesData> m_bytes_data;
	SliceType m_slice_type;
	MBType m_mb_type;
	uint8_t m_cbp;
};

__codec_end