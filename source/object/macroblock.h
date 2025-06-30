#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "mb_type.h"
#include "mb_neighbors.h"
#include "position.h"
#include "block_data.h"

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

	void ObtainLeftAndUpEdge(std::vector<uint8_t>& left_data, std::vector<uint8_t>& up_data, uint8_t& left_up_element);

	std::vector<uint8_t> GetRightData();

	std::vector<uint8_t> GetDownData();

	std::shared_ptr<Macroblock> GetMacroblock(uint32_t mb_addr);

	BlockData<16, 16> GetOriginalBlockData() const;

	BlockData<16, 16> GetReconstructedBlockData() const;

	int GetCost() const;

private:
	void Init();

	void ComputePosition();

	void ObtainOriginalData();

	void PreEncode();

	void DoEncode();

	void PostEncode();

	void IntraPredict();

	std::shared_ptr<Intra16Quantizer> TransformAndQuantizeIntra16();

	void InverseTransformAndQuantizeIntra16(std::shared_ptr<Intra16Quantizer> quantizer);

private:
	uint32_t m_addr;
	std::weak_ptr<Slice> m_slice;
	std::shared_ptr<EncoderContext> m_encoder_context;

	Position m_pos_in_mb;
	Position m_pos;
	std::unique_ptr<MBNeighbors> m_neighbors;
	uint32_t m_qp;
	MBType m_type;

	int m_cost;//prediction cost
	BlockData<16, 16, int32_t> m_diff_data;
	BlockData<16, 16> m_predicted_data;
	BlockData<16, 16> m_reconstructed_data;

	BlockData<16, 16> m_luma_data;
};

__codec_end