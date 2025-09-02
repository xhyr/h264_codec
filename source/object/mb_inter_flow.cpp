#include "mb_inter_flow.h"

#include "mb_inter_rd_optimizer.h"

__codec_begin

MBInterFlow::MBInterFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context):
	MBFlowBase(mb, encoder_context), m_rd_optimizer(std::make_unique<MBInterRDOptimizer>(mb, encoder_context))
{
}

MBInterFlow::~MBInterFlow()
{
}

void MBInterFlow::DoEncode()
{
	m_rd_optimizer->Encode();
}

void MBInterFlow::Binary(std::shared_ptr<BytesData> bytes_data)
{
	m_rd_optimizer->Binary(bytes_data);
}

__codec_end