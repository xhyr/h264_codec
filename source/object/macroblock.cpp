#include "macroblock.h"

#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"
#include "slice.h"
#include "intra16_luma_flow.h"
#include "intra4_luma_flow.h"
#include "bytes_data.h"
#include "rdo_util.h"

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
	m_bytes_data = bytes_data;

	PreEncode();
	DoEncode();
	PostEncode();
	return true;
}

std::shared_ptr<BytesData> Macroblock::GetBytesData() const
{
	return m_bytes_data;
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

BlockData<16, 16> Macroblock::GetOriginalLumaBlockData16x16() const
{
    return m_luma_data;
}

BlockData<4, 4> Macroblock::GetOriginalLumaBlockData4x4(uint32_t x_in_block, uint32_t y_in_block) const
{
	return m_luma_data.GetBlock4x4<uint8_t>(x_in_block, y_in_block);
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
	m_neighbors = std::make_unique<MBNeighbors>(shared_from_this(), m_addr, m_encoder_context);
	
	auto slice = m_slice.lock();
	m_qp = slice->GetQP();

	m_encoder_context->lambda = RDOUtil::GetLambda(m_qp);
}

void Macroblock::DoEncode()
{
	DecideLumaMode();
}

void Macroblock::DecideLumaMode()
{
	auto intra4_luma_flow = std::make_unique<Intra4LumaFlow>(shared_from_this(), m_encoder_context);
	intra4_luma_flow->Frontend();
	int intra4_cost = intra4_luma_flow->GetCost();

	auto intra16_luma_flow = std::make_unique<Intra16LumaFlow>(shared_from_this(), m_encoder_context);
	intra16_luma_flow->Frontend();
	int intra16_cost = intra16_luma_flow->GetCost();

	if (intra4_cost <= intra16_cost)
	{
		m_type = MBType::I4;
		m_intra4_luma_prediction_types = intra4_luma_flow->GetPredictionTypes();
		m_intra_luma_flow.reset(intra4_luma_flow.release());
	}
	else
	{
		m_type = MBType::I16;
		m_intra16_prediction_type = intra16_luma_flow->GetPredictionType();
		m_intra_luma_flow.reset(intra16_luma_flow.release());
	}

	m_reconstructed_luma_data = m_intra_luma_flow->GetReconstructedData();
	m_luma_cbp = m_intra_luma_flow->GetCBP();
}

void Macroblock::PostEncode()
{
	
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

int Macroblock::GetQP() const
{
	return m_qp;
}

std::shared_ptr<Slice> Macroblock::GetSlice()
{
	return m_slice.lock();
}

MBType Macroblock::GetType() const
{
	return m_type;
}

Intra4LumaPredictionType Macroblock::GetIntra4LumaPredicionType(uint32_t x_in_block, uint32_t y_in_block) const
{
	return m_intra4_luma_prediction_types[x_in_block + 4 * y_in_block];
}

__codec_end