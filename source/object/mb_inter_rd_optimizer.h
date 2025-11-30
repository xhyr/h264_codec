#pragma once

#include <memory>

#include "mb_type.h"

__codec_begin

class Macroblock;
struct EncoderContext;
class BytesData;
class InterLumaFlowBase;
class InterChromaFlow;

class MBInterRDOptimizer final
{
public:
	MBInterRDOptimizer(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~MBInterRDOptimizer();

	void Encode();

	uint64_t Binary(std::shared_ptr<BytesData> bytes_data, bool rdo_pass = false);

private:
	void OutputMB(std::shared_ptr<BytesData> bytes_data, bool rdo_pass);

private:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;

	std::shared_ptr<InterLumaFlowBase> m_luma_flow;
	std::shared_ptr<InterChromaFlow> m_chroma_flow;

	uint32_t m_mb_addr;
	int m_chroma_cbp{ 0 };
	int m_luma_cbp{ 0 };
	int m_cbp{ 0 };
	MBType m_mb_type;
	int m_rd_cost{ 0 };
};

__codec_end