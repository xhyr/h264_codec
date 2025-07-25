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
#include "cavlc_coder_luma_16x16.h"
#include "intra8_chroma_predictor.h"
#include "intra8_chroma_transformer.h"
#include "intra8_chroma_quantizer.h"
#include "inverse_intra8_chroma_quantizer.h"
#include "intra8_chroma_reconstructor.h"
#include "cavlc_coder_chroma_8x8.h"
#include "mb_util.h"
#include "bytes_data.h"
#include "mb_binaryer.h"
#include "intra4_luma_predictor.h"

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
	m_type = MBType::I16;
	m_neighbors = std::make_unique<MBNeighbors>(shared_from_this(), m_addr, m_encoder_context);
	
	auto slice = m_slice.lock();
	m_qp = slice->GetQP();
}

void Macroblock::DoEncode()
{
	//luma
	auto luma_diff_data = IntraLumaPredict();
	auto luma_quantizer = TransformAndQuantizeIntra16Luma(luma_diff_data);
	DoCodeCavlcLuma(luma_quantizer);
	InverseTransformAndQuantizeIntra16Luma(luma_quantizer);
	
	//chroma
	auto [cb_diff_data, cr_diff_data] = IntraChromaPredict();
	std::vector<PlaneType> plane_types{ PlaneType::Cb, PlaneType::Cr };
	for (auto plane_type : plane_types)
	{
		auto diff_data = plane_type == PlaneType::Cb ? cb_diff_data : cr_diff_data;
		auto quantizer = TransformAndQuantizeIntra8Chroma(diff_data);
		DoCodeCavlcChroma(quantizer, plane_type);
		InverseTransformAndQuantizeIntra8Chroma(quantizer, plane_type);
	}

	//cbp
	m_cbp = m_luma_cbp | (m_chroma_cbp << 4);
}

void Macroblock::PostEncode()
{
	if(m_type == MBType::I16)
		m_intra16_offset = MBUtil::CalculateIntra16Offset(m_cbp, m_intra16_luma_prediction_type);

	Convert2Binary();
}

BlockData<16, 16, int32_t> Macroblock::IntraLumaPredict()
{
	Intra4Predictor intra4_predictor(shared_from_this(), m_encoder_context);
	intra4_predictor.Decide();
	

	Intra16Predictor intra16_predictor(shared_from_this(), m_encoder_context);
	m_intra16_luma_prediction_type = intra16_predictor.Decide();
	m_luma_cost = intra16_predictor.GetCost();
	m_predicted_luma_data = intra16_predictor.GetPredictedData();
	return intra16_predictor.GetDiffData();
}

std::shared_ptr<Intra16LumaQuantizer> Macroblock::TransformAndQuantizeIntra16Luma(const BlockData<16, 16, int32_t>& diff_data)
{
	Intra16LumaTransformer transformer(diff_data);
	transformer.Transform();

	auto quantizer = std::make_shared<Intra16LumaQuantizer>(m_qp, transformer.GetDCBlock(), transformer.GetBlocks());
	quantizer->Quantize();

	return quantizer;
}

void Macroblock::DoCodeCavlcLuma(const std::shared_ptr<Intra16LumaQuantizer>& quantizer)
{
	CavlcCoderLuma16x16 coder;
	coder.Code(quantizer->GetDCBlock(), quantizer->GetACBlocks());
	m_luma_cbp = coder.GetCodedBlockPattern();
	m_cavlc_data_source.luma_dc = coder.GetDCLevelAndRuns();
	m_cavlc_data_source.luma_acs = coder.GetACLevelAndRuns();
}

void Macroblock::InverseTransformAndQuantizeIntra16Luma(const std::shared_ptr<Intra16LumaQuantizer>& quantizer)
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

std::pair<BlockData<8, 8, int32_t>, BlockData<8, 8, int32_t>> Macroblock::IntraChromaPredict()
{
	Intra8ChromaPredictor predictor(shared_from_this(), m_encoder_context);
	m_intra_chroma_prediction_type = predictor.Decide();
	m_predicted_cb_data = predictor.GetPredictedData(PlaneType::Cb);
	m_predicted_cr_data = predictor.GetPredictedData(PlaneType::Cr);
	return std::make_pair(predictor.GetDiffData(PlaneType::Cb), predictor.GetDiffData(PlaneType::Cr));
}

std::shared_ptr<Intra8ChromaQuantizer> Macroblock::TransformAndQuantizeIntra8Chroma(const BlockData<8, 8, int32_t>& diff_data)
{
	Intra8ChromaTransformer transformer(diff_data);
	transformer.Transform();

	auto quantizer = std::make_shared<Intra8ChromaQuantizer>(m_qp, transformer.GetDCBlock(), transformer.GetBlocks());
	quantizer->Quantize();
	return quantizer;
}

void Macroblock::InverseTransformAndQuantizeIntra8Chroma(const std::shared_ptr<Intra8ChromaQuantizer>& quantizer, PlaneType plane_type)
{
	auto dc_block = quantizer->GetDCBlock();
	dc_block = TransformUtil::InverseHadamard(dc_block);

	InverseIntra8ChromaQuantizer inverse_quantizer(m_qp, dc_block, quantizer->GetACBlocks());
	inverse_quantizer.InverseQuantize();

	auto blocks = inverse_quantizer.GetBlocks();
	for (auto& block : blocks)
		block = TransformUtil::InverseDCT(block);

	Intra8ChromaReconstructor reconstructor(blocks, plane_type == PlaneType::Cb ? m_predicted_cb_data : m_predicted_cr_data);
	reconstructor.Reconstruct();
	auto& reconstructed_data = plane_type == PlaneType::Cb ? m_reconstructed_cb_data : m_reconstructed_cr_data;
	reconstructed_data = reconstructor.GetBlockData();
}

void Macroblock::DoCodeCavlcChroma(const std::shared_ptr<Intra8ChromaQuantizer>& quantizer, PlaneType plane_type)
{
	CavlcCoderChroma8x8 coder;
	coder.Code(quantizer->GetDCBlock(), quantizer->GetACBlocks());
	
	m_chroma_cbp = std::max(m_chroma_cbp, coder.GetCodedBlockPattern());

	if (coder.HasResetCofficients())
		quantizer->ResetACToZeros();

	if (plane_type == PlaneType::Cb)
	{
		m_cavlc_data_source.cb_dc = coder.GetDCLevelAndRuns();
		m_cavlc_data_source.cb_acs = coder.GetACLevelAndRuns();
	}
	else
	{
		m_cavlc_data_source.cr_dc = coder.GetDCLevelAndRuns();
		m_cavlc_data_source.cr_acs = coder.GetACLevelAndRuns();
	}
}

void Macroblock::Convert2Binary()
{
	MBBinaryer binaryer(m_slice, m_addr, m_bytes_data);
	binaryer.OutputMBType(m_type, m_intra16_offset);
	binaryer.OutputChromaPredMode(m_intra_chroma_prediction_type);
	binaryer.OutputCBP(m_cbp);
	binaryer.OutputQPDelta(0);
	binaryer.OutputLumaCoeffs(m_cavlc_data_source);
	binaryer.OutputChromaCoeffs(m_cavlc_data_source);
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

__codec_end