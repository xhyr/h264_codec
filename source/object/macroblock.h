#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "mb_type.h"
#include "mb_neighbors.h"
#include "position.h"
#include "block_data.h"
#include "common_types.h"

__codec_begin

class Slice;
struct EncoderContext;
class Intra16Quantizer;

class Macroblock : public std::enable_shared_from_this<Macroblock>
{
public:
	Macroblock(uint32_t mb_addr, std::weak_ptr<Slice> slice, std::shared_ptr<EncoderContext> encoder_context);
	~Macroblock();

	bool Encode();

	void ObtainLeftAndUpEdge(std::vector<uint8_t>& left_data, std::vector<uint8_t>& up_data, uint8_t& left_up_element, PlaneType plane_type);

	std::vector<uint8_t> GetRightData(PlaneType plane_type);

	std::vector<uint8_t> GetDownData(PlaneType plane_type);

	std::shared_ptr<Macroblock> GetMacroblock(uint32_t mb_addr);

	BlockData<16, 16> GetOriginalLumaBlockData() const;

	BlockData<8, 8> GetOriginalChromaBlockData(PlaneType plane_type) const;

	BlockData<16, 16> GetReconstructedBlockData() const;

	int GetCost() const;

private:
	void Init();

	void ComputePosition();

	void ObtainOriginalData();

	void PreEncode();

	void DoEncode();

	void PostEncode();

	BlockData<16, 16, int> IntraLumaPredict();

	std::shared_ptr<Intra16Quantizer> TransformAndQuantizeIntra16Luma(const BlockData<16, 16, int>& diff_data);

	void DoCodeCavlcLuma(const std::shared_ptr<Intra16Quantizer>& quantizer);

	void InverseTransformAndQuantizeIntra16Luma(const std::shared_ptr<Intra16Quantizer>& quantizer);

	void IntraChromaPredict();

private:
	uint32_t m_addr;
	std::weak_ptr<Slice> m_slice;
	std::shared_ptr<EncoderContext> m_encoder_context;

	Position m_pos_in_mb;
	Position m_pos;
	std::unique_ptr<MBNeighbors> m_neighbors;
	uint32_t m_qp;
	MBType m_type;
	uint8_t m_cbp;

	int m_luma_cost;//prediction cost
	BlockData<16, 16> m_predicted_luma_data;

	BlockData<16, 16> m_reconstructed_luma_data;
	BlockData<8, 8> m_reconstructed_cb_data;
	BlockData<8, 8> m_reconstructed_cr_data;

	BlockData<16, 16> m_luma_data;
	BlockData<8, 8> m_cb_data;
	BlockData<8, 8> m_cr_data;
};

__codec_end