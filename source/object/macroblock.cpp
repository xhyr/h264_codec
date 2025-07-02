#include "macroblock.h"

#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"
#include "slice.h"
#include "intra16_predictor.h"
#include "intra16_transformer.h"
#include "intra16_quantizer.h"
#include "inverse_intra16_quantizer.h"
#include "transform_util.h"
#include "intra16_reconstructor.h"
#include "cavlc_coder_16x16.h"

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
	return m_reconstructed_data.GetRightData();
}

std::vector<uint8_t> Macroblock::GetDownData()
{
	return m_reconstructed_data.GetDownData();
}

std::shared_ptr<Macroblock> Macroblock::GetMacroblock(uint32_t mb_addr)
{
	auto slice = m_slice.lock();
	return slice->GetMacroblock(mb_addr);
}

BlockData<16, 16> Macroblock::GetOriginalBlockData() const
{
    return m_luma_data;
}

BlockData<16, 16> Macroblock::GetReconstructedBlockData() const
{
	return m_reconstructed_data;
}

int Macroblock::GetCost() const
{
	return m_cost;
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
}

void Macroblock::PreEncode()
{
	m_neighbors = std::make_unique<MBNeighbors>(shared_from_this(), m_addr, m_encoder_context);

	m_qp = 28;
}

void Macroblock::DoEncode()
{
	IntraPredict();

	auto quantizer = TransformAndQuantizeIntra16();

	DoCodeCavlc(quantizer);

	InverseTransformAndQuantizeIntra16(quantizer);
	
}

void Macroblock::PostEncode()
{

}

void Macroblock::IntraPredict()
{
	Intra16Predictor intra16_predictor(shared_from_this(), m_encoder_context);
	intra16_predictor.Decide();
	m_cost = intra16_predictor.GetCost();
	m_diff_data = intra16_predictor.GetDiffData();
	m_predicted_data = intra16_predictor.GetPredictedData();
}

std::shared_ptr<Intra16Quantizer> Macroblock::TransformAndQuantizeIntra16()
{
	Intra16Transformer transformer(m_diff_data);
	transformer.Transform();

	auto quantizer = std::make_shared<Intra16Quantizer>(m_qp, transformer.GetDCBlock(), transformer.GetBlocks());
	quantizer->Quantize();
	return quantizer;
}

void Macroblock::DoCodeCavlc(const std::shared_ptr<Intra16Quantizer>& quantizer)
{
	CavlcCoder16x16 coder;
	coder.Code(quantizer->GetDCBlock(), quantizer->GetACBlocks());
	m_cbp = coder.GetCodedBlockPattern();
}

void Macroblock::InverseTransformAndQuantizeIntra16(const std::shared_ptr<Intra16Quantizer>& quantizer)
{
	auto dc_block = quantizer->GetDCBlock();
	dc_block = TransformUtil::InverseHadamard(dc_block);
	
	InverseIntra16Quantizer inverse_quantizer(m_qp, dc_block, quantizer->GetACBlocks());
	inverse_quantizer.InverseQuantize();

	auto blocks = inverse_quantizer.GetBlocks();
	for (auto& block : blocks)
		block = TransformUtil::InverseDCT(block);

	//reconstruct
	Intra16Reconstructor reconstructor(blocks, m_predicted_data);
	reconstructor.Reconstruct();
	m_reconstructed_data = reconstructor.GetBlockData();
}

__codec_end