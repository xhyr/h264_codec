#include "mb_inter_flow.h"

#include "mb_intra_rd_optimizer.h"
#include "mb_inter_rd_optimizer.h"
#include "macroblock.h"

#define TRACY_ENABLE
#include <tracy/Tracy.hpp>

__codec_begin

MBInterFlow::MBInterFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context):
	MBFlowBase(mb, encoder_context), 
	m_intra_rd_optimizer(std::make_unique<MBIntraRDOptimizer>(mb, encoder_context)),
	m_inter_rd_optimizer(std::make_unique<MBInterRDOptimizer>(mb, encoder_context))
{
}

MBInterFlow::~MBInterFlow()
{
}

void MBInterFlow::DoEncode()
{
	ZoneScoped;

	int64_t min_rd_cost = std::numeric_limits<int64_t>::max();
	m_inter_rd_optimizer->Encode(min_rd_cost);
	//m_is_intra = m_intra_rd_optimizer->Encode(min_rd_cost);
}

void MBInterFlow::Binary(std::shared_ptr<BytesData> bytes_data)
{
	if (m_is_intra)
		m_intra_rd_optimizer->Binary(bytes_data);
	else
		m_inter_rd_optimizer->Binary(bytes_data);
}

__codec_end