#include "macroblock.h"

#include "encoder_context.h"
#include "yuv_frame.h"
#include "data_util.h"
#include "slice.h"
#include "intra16_luma_predictor.h"
#include "intra16_luma_transformer.h"
#include "intra16_luma_quantizer.h"
#include "inverse_intra16_luma_quantizer.h"
#include "transform_util.h"
#include "intra16_luma_reconstructor.h"
#include "cavlc_coder_16x16.h"
#include "intra8_chroma_predictor.h"

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

BlockData<16, 16> Macroblock::GetOriginalLumaBlockData() const
{
    return m_luma_data;
}

BlockData<8, 8> Macroblock::GetOriginalChromaBlockData(PlaneType plane_type) const
{
	return plane_type == PlaneType::Cb ? m_cb_data : m_cr_data;
}

BlockData<16, 16> Macroblock::GetReconstructedBlockData() const
{
	return m_reconstructed_luma_data;
}

int Macroblock::GetCost() const
{
	return m_luma_cost;
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

	m_qp = 28;
}

void Macroblock::DoEncode()
{
	//luma
	auto luma_diff_data = IntraLumaPredict();
	auto quantizer = TransformAndQuantizeIntra16Luma(luma_diff_data);
	DoCodeCavlcLuma(quantizer);
	InverseTransformAndQuantizeIntra16Luma(quantizer);
	
	//chroma
	IntraChromaPredict();
}

void Macroblock::PostEncode()
{

}

BlockData<16, 16, int> Macroblock::IntraLumaPredict()
{
	Intra16Predictor intra16_predictor(shared_from_this(), m_encoder_context);
	intra16_predictor.Decide();
	m_luma_cost = intra16_predictor.GetCost();
	m_predicted_luma_data = intra16_predictor.GetPredictedData();
	return intra16_predictor.GetDiffData();
}

std::shared_ptr<Intra16Quantizer> Macroblock::TransformAndQuantizeIntra16Luma(const BlockData<16, 16, int>& diff_data)
{
	Intra16LumaTransformer transformer(diff_data);
	transformer.Transform();

	auto quantizer = std::make_shared<Intra16Quantizer>(m_qp, transformer.GetDCBlock(), transformer.GetBlocks());
	quantizer->Quantize();
	return quantizer;
}

void Macroblock::DoCodeCavlcLuma(const std::shared_ptr<Intra16Quantizer>& quantizer)
{
	CavlcCoder16x16 coder;
	coder.Code(quantizer->GetDCBlock(), quantizer->GetACBlocks());
	m_cbp = coder.GetCodedBlockPattern();
}

void Macroblock::InverseTransformAndQuantizeIntra16Luma(const std::shared_ptr<Intra16Quantizer>& quantizer)
{
	auto dc_block = quantizer->GetDCBlock();
	dc_block = TransformUtil::InverseHadamard(dc_block);
	
	InverseIntra16LumaQuantizer inverse_quantizer(m_qp, dc_block, quantizer->GetACBlocks());
	inverse_quantizer.InverseQuantize();

	auto blocks = inverse_quantizer.GetBlocks();
	for (auto& block : blocks)
		block = TransformUtil::InverseDCT(block);

	//reconstruct
	Intra16LumaReconstructor reconstructor(blocks, m_predicted_luma_data);
	reconstructor.Reconstruct();
	m_reconstructed_luma_data = reconstructor.GetBlockData();
}

void Macroblock::IntraChromaPredict()
{
	Intra8ChromaPredictor predictor(shared_from_this(), m_encoder_context);
	predictor.Decide();
}

__codec_end