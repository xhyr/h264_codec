#pragma once

#include <memory>

#include "slice_type.h"

__codec_begin

class Macroblock;
struct EncoderContext;
class BytesData;
class MBNeighbors;

class MBFlowBase
{
public:
	explicit MBFlowBase(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	virtual ~MBFlowBase();

	void PreEncode();

	void PostEncode();

	virtual void DoEncode() = 0;

	virtual void Binary(std::shared_ptr<BytesData> bytes_data) = 0;

private:
	void InitLambdas();

protected:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;

	uint32_t m_mb_addr;
	std::unique_ptr<MBNeighbors> m_neighbors;
	int m_qp;

public:
	static std::unique_ptr<MBFlowBase> CreateMBFlow(SliceType slice_type, std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);

};



__codec_end
