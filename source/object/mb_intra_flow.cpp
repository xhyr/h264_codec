#include "mb_intra_flow.h"

#include "mb_intra_rd_optimizer.h"

__codec_begin

MBIntraFlow::MBIntraFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	MBFlowBase(mb, encoder_context), m_rd_optimizer(std::make_unique<MBIntraRDOptimizer>(mb, encoder_context))
{
}

MBIntraFlow::~MBIntraFlow()
{
}

void MBIntraFlow::DoEncode()
{
	m_rd_optimizer->Encode();
}

void MBIntraFlow::Binary(std::shared_ptr<BytesData> bytes_data)
{
	m_rd_optimizer->Binary(bytes_data);
}

__codec_end
