#include "mb_flow_base.h"

#include "macroblock.h"
#include "mb_neighbors.h"
#include "slice.h"
#include "encoder_context.h"
#include "rdo_util.h"
#include "mb_intra_flow.h"
#include "mb_inter_flow.h"

__codec_begin

MBFlowBase::MBFlowBase(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) : 
	m_mb(mb), m_encoder_context(encoder_context)
{
}

MBFlowBase::~MBFlowBase()
{
}

void MBFlowBase::PreEncode()
{
	m_mb_addr = m_mb->GetAddress();
	m_qp = m_mb->GetSlice()->GetQP();
	
	m_encoder_context->mb_addr = m_mb_addr;
	m_encoder_context->qp = m_qp;
	m_mb->SetQP(m_qp);
	
	InitLambdas();
}

void MBFlowBase::PostEncode()
{

}

void MBFlowBase::InitLambdas()
{
	m_encoder_context->lambda_mode = RDOUtil::GetLambdaMode(m_qp);
	m_encoder_context->lambda_motion = RDOUtil::GetLambdaMotion(m_qp);
	m_encoder_context->lambda_mode_fp = RDOUtil::GetLambdaModeFixedPoint(m_qp);
	m_encoder_context->lambda_motion_fp = RDOUtil::GetLambdaMotionFixedPoint(m_qp);
}


std::unique_ptr<MBFlowBase> MBFlowBase::CreateMBFlow(SliceType slice_type, std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context)
{
	std::unique_ptr<MBFlowBase> mb_flow;
	switch (slice_type)
	{
	case codec::SliceType::P:
		mb_flow.reset(new MBInterFlow(mb, encoder_context));
		break;
	case codec::SliceType::I:
		mb_flow.reset(new MBIntraFlow(mb, encoder_context));
		break;
	default:
		break;
	}

	return mb_flow;
}

__codec_end

