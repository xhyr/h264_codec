#pragma once

#include <memory>
#include <vector>

#include "mb_type.h"
#include "prediction_type.h"

__codec_begin

class Macroblock;
struct EncoderContext;
class BytesData;
class ChromaFlow;
class IntraLumaFlowBase;

class RDOptimizer
{
public:
	RDOptimizer(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, std::shared_ptr<BytesData> bytes_data);
	~RDOptimizer();

	void Frontend();

	void Backend();

private:
	double CalculateRDCost(MBType mb_type);

	int CalculateRate(MBType mb_type);

private:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	std::shared_ptr<BytesData> m_bytes_data;

	std::shared_ptr<ChromaFlow> m_chroma_flow;
	std::shared_ptr<IntraLumaFlowBase> m_luma_flow;
	std::shared_ptr<BytesData> m_tmp_bytes_data;
	int m_chroma_cbp{ 0 };
	int m_luma_cbp{ 0 };
	int m_cbp{ 0 };
	std::shared_ptr<BytesData> m_best_bytes_data;
	MBType m_mb_type;
	double m_rd_cost{ 0 };
};

__codec_end