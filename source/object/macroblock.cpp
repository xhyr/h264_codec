#include "macroblock.h"

#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"
#include "slice.h"
#include "constant_values.h"
#include "intra16_predictor.h"

__codec_begin

Macroblock::Macroblock(uint32_t mb_addr, std::weak_ptr<Slice> slice, std::shared_ptr<EncoderContext> encoder_context):
	m_addr(mb_addr), m_slice(slice), m_encoder_context(encoder_context)
{
	Init();
}

Macroblock::~Macroblock()
{
}

bool Macroblock::Encode()
{
	PreEncode();
	DoEncode();
	PostEncode();
	return true;
}

void Macroblock::ObtainLeftAndUpEdge(std::vector<uint8_t>& left_data, std::vector<uint8_t>& up_data, uint8_t& left_up_element)
{
	m_neighbors->ObtainLeftAndUpEdge(left_data, up_data, left_up_element);
}

std::vector<uint8_t> Macroblock::GetRightData()
{
	return m_luma_data.GetRightData();
}

std::vector<uint8_t> Macroblock::GetDownData()
{
	return m_luma_data.GetDownData();
}

std::shared_ptr<Macroblock> Macroblock::GetMacroblock(uint32_t mb_addr)
{
	auto slice = m_slice.lock();
	return slice->GetMacroblock(mb_addr);
}

BlockData<ConstantValues::s_mb_width, ConstantValues::s_mb_height> Macroblock::GetBlockData() const
{
    return m_luma_data;
}

int Macroblock::GetCost() const
{
	return m_cost;
}

void Macroblock::Init()
{
	ComputePosition();
	ObtainData();
}

void Macroblock::ComputePosition()
{
	m_pos_in_mb.x = m_addr % m_encoder_context->width_in_mb;
	m_pos_in_mb.y = m_addr / m_encoder_context->width_in_mb;
	m_pos.x = m_pos_in_mb.x << 4;
	m_pos.y = m_pos_in_mb.y << 4;
}

void Macroblock::ObtainData()
{
	//luma
	auto frame_luma_data = m_encoder_context->yuv_frame->y_data;
	m_luma_data.SetData(DataUtil::ObtainDataInBlock(frame_luma_data, m_pos.x, m_pos.y, ConstantValues::s_mb_width, ConstantValues::s_mb_height, m_encoder_context->width));
}

void Macroblock::PreEncode()
{
	m_neighbors = std::make_unique<MBNeighbors>(shared_from_this(), m_addr, m_encoder_context);

	m_qp = 28;
}

void Macroblock::DoEncode()
{
	IntraPredict();
}

void Macroblock::PostEncode()
{

}

void Macroblock::IntraPredict()
{
	Intra16Predictor intra16_predictor(shared_from_this(), m_encoder_context);
	intra16_predictor.Decide();
	m_cost = intra16_predictor.GetCost();
}

__codec_end