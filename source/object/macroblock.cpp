#include "macroblock.h"

#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"
#include "slice.h"
#include "mb_flow_base.h"
#include "bytes_data.h"
#include "rdo_util.h"
#include "intra_chroma_flow.h"
#include "mb_util.h"
#include "motion_info.h"
#include "motion_info_context.h"

#define TRACY_ENABLE
#include <tracy/Tracy.hpp>

__codec_begin

Macroblock::Macroblock(uint32_t mb_addr, std::weak_ptr<Slice> slice, std::shared_ptr<EncoderContext> encoder_context):
	m_addr(mb_addr), m_slice(slice), m_encoder_context(encoder_context)
{
	Init();
}

Macroblock::~Macroblock()
{
}

bool Macroblock::Encode(std::shared_ptr<BytesData> bytes_data)
{
	ZoneScoped;

	m_bytes_data = bytes_data;

	PreEncode();
	DoEncode();
	Binary();
	PostEncode();
	return true;
}

void Macroblock::ObtainLeftAndUpEdge(std::vector<uint8_t>& left_data, std::vector<uint8_t>& up_data, uint8_t& left_up_element, PlaneType plane_type)
{
	m_neighbors->ObtainLeftAndUpEdge(left_data, up_data, left_up_element, plane_type);
}

std::vector<uint8_t> Macroblock::GetRightData(PlaneType plane_type)
{
	if (plane_type == PlaneType::Luma)
		return m_reconstructed_luma_data.GetRightData();
	else if (plane_type == PlaneType::Cb)
		return m_reconstructed_cb_data.GetRightData();
	else 
		return m_reconstructed_cr_data.GetRightData();
}

std::vector<uint8_t> Macroblock::GetDownData(PlaneType plane_type)
{
	if (plane_type == PlaneType::Luma)
		return m_reconstructed_luma_data.GetDownData();
	else if (plane_type == PlaneType::Cb)
		return m_reconstructed_cb_data.GetDownData();
	else
		return m_reconstructed_cr_data.GetDownData();
}

std::shared_ptr<Macroblock> Macroblock::GetMacroblock(uint32_t mb_addr)
{
	auto slice = m_slice.lock();
	return slice->GetMacroblock(mb_addr);
}

std::shared_ptr<Macroblock> Macroblock::GetLeftMacroblock()
{
	if (m_addr % m_encoder_context->width_in_mb == 0)
		return nullptr;
	else
		return GetMacroblock(m_addr - 1);
}

std::shared_ptr<Macroblock> Macroblock::GetUpMacroblock()
{
	if (m_addr / m_encoder_context->width_in_mb == 0)
		return nullptr;
	else
		return GetMacroblock(m_addr - m_encoder_context->width_in_mb);
}

BlockData<16, 16> Macroblock::GetOriginalLumaBlockData() const
{
	return m_luma_data;
}

BlockData<8, 8> Macroblock::GetOriginalChromaBlockData(PlaneType plane_type) const
{
	return plane_type == PlaneType::Cb ? m_cb_data : m_cr_data;
}

BlockData<16, 16> Macroblock::GetReconstructedLumaBlockData() const
{
	return m_reconstructed_luma_data;
}

BlockData<8, 8> Macroblock::GetReconstructedChromaBlockData(PlaneType plane_type) const
{
	return plane_type == PlaneType::Cb ? m_reconstructed_cb_data : m_reconstructed_cr_data;
}

void Macroblock::SetReconstructedLumaBlockData(const BlockData<16, 16>& block_data)
{
	m_reconstructed_luma_data = block_data;
}

void Macroblock::SetReconstructedChromaBlockData(const BlockData<8, 8>& block_data, PlaneType plane_type)
{
	(plane_type == PlaneType::Cb ? m_reconstructed_cb_data : m_reconstructed_cr_data) = block_data;
}

void Macroblock::Init()
{
	ComputePosition();
	ObtainOriginalData();
}

void Macroblock::ComputePosition()
{
	m_pos_in_mb.x = m_addr % m_encoder_context->width_in_mb;
	m_pos_in_mb.y = m_addr / m_encoder_context->width_in_mb;
	m_pos.x = m_pos_in_mb.x << 4;
	m_pos.y = m_pos_in_mb.y << 4;
}

void Macroblock::ObtainOriginalData()
{
	//luma
	auto frame_luma_data = m_encoder_context->yuv_frame->y_data;
	m_luma_data.SetData(DataUtil::ObtainDataInBlock(frame_luma_data, m_pos.x, m_pos.y, 16, 16, m_encoder_context->width));
	
	//cb
	auto frame_cb_data = m_encoder_context->yuv_frame->u_data;
	m_cb_data.SetData(DataUtil::ObtainDataInBlock(frame_cb_data, m_pos.x >> 1, m_pos.y >> 1, 8, 8, m_encoder_context->width >> 1));

	//cr
	auto frame_cr_data = m_encoder_context->yuv_frame->v_data;
	m_cr_data.SetData(DataUtil::ObtainDataInBlock(frame_cr_data, m_pos.x >> 1, m_pos.y >> 1, 8, 8, m_encoder_context->width >> 1));
}

void Macroblock::PreEncode()
{
	ZoneScoped;

	m_neighbors = std::make_unique<MBNeighbors>(shared_from_this(), m_addr, m_encoder_context);

	CreateMBFlow();
	m_mb_flow->PreEncode();
}

void Macroblock::DoEncode()
{
	ZoneScoped;

	m_mb_flow->DoEncode();
}

void Macroblock::Binary()
{
	ZoneScoped;

	m_mb_flow->Binary(m_bytes_data);
}

void Macroblock::PostEncode()
{
	ZoneScoped;

	m_mb_flow->PostEncode();
}

void Macroblock::CreateMBFlow()
{
	auto slice_type = m_slice.lock()->GetType();
	m_mb_flow = MBFlowBase::CreateMBFlow(slice_type, shared_from_this(), m_encoder_context);
}

uint32_t Macroblock::GetAddress() const
{
	return m_addr;
}

std::pair<uint32_t, uint32_t> Macroblock::GetPositionInMb() const
{
	return std::make_pair(m_addr % m_encoder_context->width_in_mb, m_addr / m_encoder_context->width_in_mb);
}

std::pair<uint32_t, uint32_t> Macroblock::GetPosition() const
{
	return std::make_pair(m_addr % m_encoder_context->width_in_mb * 16, m_addr / m_encoder_context->width_in_mb * 16);
}

void Macroblock::SetQP(int qp)
{
	m_qp = qp;
}

int Macroblock::GetQP() const
{
	return m_qp;
}

std::shared_ptr<Slice> Macroblock::GetSlice()
{
	return m_slice.lock();
}

void Macroblock::SetType(MBType mb_type)
{
	m_type = mb_type;
}

MBType Macroblock::GetType() const
{
	return m_type;
}

Intra4LumaPredictionType Macroblock::GetIntra4LumaPredicionType(uint32_t x_in_block, uint32_t y_in_block) const
{
	return m_intra4_luma_prediction_types[x_in_block + 4 * y_in_block];
}

void Macroblock::SetIntra4LumaPredictionTypes(const std::vector<Intra4LumaPredictionType>& prediction_types)
{
	m_intra4_luma_prediction_types = prediction_types;
}

MotionInfo Macroblock::GetMotionInfo(uint32_t x_in_block, uint32_t y_in_block)
{
	return m_encoder_context->motion_info_context->GetMotionInfo(m_addr, x_in_block, y_in_block);
}

void Macroblock::SetLumaDetailedCBP(uint32_t value)
{
	m_luma_detailed_cbp = value;
}

uint32_t Macroblock::GetLumaDetailedCBP() const
{
	return m_luma_detailed_cbp;
}

__codec_end