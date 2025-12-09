#pragma once

#include <memory>
#include <vector>

#include "mb_type.h"
#include "prediction_type.h"

__codec_begin

class Macroblock;
struct EncoderContext;
class BytesData;
class IntraChromaFlow;
class IntraLumaFlowBase;

class MBIntraRDOptimizer
{
public:
	MBIntraRDOptimizer(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~MBIntraRDOptimizer();

	bool Encode(int64_t& min_rd_cost);

	uint32_t Binary(std::shared_ptr<BytesData> bytes_data);

private:
	int64_t CalculateRDCost(MBType mb_type, int64_t min_rd_cost);

	void RunLumaFlow(MBType mb_type);

	int64_t CalculateRate(MBType mb_type);

	int OutputMB(MBType mb_type, std::shared_ptr<BytesData> bytes_data);

private:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;

	std::shared_ptr<IntraChromaFlow> m_chroma_flow;
	std::shared_ptr<IntraLumaFlowBase> m_luma_flow;
	std::shared_ptr<IntraLumaFlowBase> m_best_luma_flow;

	uint32_t m_mb_addr;
	int m_chroma_cbp{ 0 };
	int m_luma_cbp{ 0 };
	int m_cbp{ 0 };
	MBType m_mb_type;
};

__codec_end