#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "mb_type.h"
#include "mb_neighbors.h"
#include "position.h"
#include "block_data.h"
#include "common_types.h"
#include "prediction_type.h"
#include "cavlc_types.h"

__codec_begin

class Slice;
struct EncoderContext;
class BytesData;
class MBFlowBase;

class Macroblock : public std::enable_shared_from_this<Macroblock>
{
public:
	Macroblock(uint32_t mb_addr, std::weak_ptr<Slice> slice, std::shared_ptr<EncoderContext> encoder_context);
	~Macroblock();

	bool Encode(std::shared_ptr<BytesData> bytes_data);

	void ObtainLeftAndUpEdge(std::vector<uint8_t>& left_data, std::vector<uint8_t>& up_data, uint8_t& left_up_element, PlaneType plane_type);

	std::vector<uint8_t> GetRightData(PlaneType plane_type);

	std::vector<uint8_t> GetDownData(PlaneType plane_type);

	std::shared_ptr<Macroblock> GetMacroblock(uint32_t mb_addr);

	std::shared_ptr<Macroblock> GetLeftMacroblock();

	std::shared_ptr<Macroblock> GetUpMacroblock();

	BlockData<16, 16> GetOriginalLumaBlockData16x16() const;

	BlockData<4, 4> GetOriginalLumaBlockData4x4(uint32_t x_in_block, uint32_t y_in_block) const;

	BlockData<8, 8> GetOriginalChromaBlockData(PlaneType plane_type) const;

	BlockData<16, 16> GetReconstructedLumaBlockData() const;

	BlockData<8, 8> GetReconstructedChromaBlockData(PlaneType plane_type) const;

	void SetReconstructedLumaBlockData(const BlockData<16, 16>& block_data);

	void SetReconstructedChromaBlockData(const BlockData<8, 8>& block_data, PlaneType plane_type);

	uint32_t GetAddress() const;

	std::pair<uint32_t, uint32_t> GetPositionInMb() const;

	std::pair<uint32_t, uint32_t> GetPosition() const;

	void SetQP(int qp);

	int GetQP() const;

	std::shared_ptr<Slice> GetSlice();

	void SetType(MBType mb_type);

	MBType GetType() const;

	Intra4LumaPredictionType GetIntra4LumaPredicionType(uint32_t x_in_block, uint32_t y_in_block) const;

	void SetIntra4LumaPredictionTypes(const std::vector<Intra4LumaPredictionType>& prediction_types);

private:
	void Init();

	void ComputePosition();

	void ObtainOriginalData();

	void PreEncode();

	void DoEncode();

	void Binary();

	void PostEncode();

	void CreateMBFlow();

private:
	uint32_t m_addr;
	std::weak_ptr<Slice> m_slice;
	std::shared_ptr<EncoderContext> m_encoder_context;

	Position m_pos_in_mb;
	Position m_pos;
	std::unique_ptr<MBNeighbors> m_neighbors;
	int m_qp;
	MBType m_type;
	std::vector<Intra4LumaPredictionType> m_intra4_luma_prediction_types;
	Intra16LumaPredictionType m_intra16_prediction_type;

	BlockData<16, 16> m_reconstructed_luma_data;
	BlockData<8, 8> m_reconstructed_cb_data;
	BlockData<8, 8> m_reconstructed_cr_data;

	BlockData<16, 16> m_luma_data;
	BlockData<8, 8> m_cb_data;
	BlockData<8, 8> m_cr_data;

	std::unique_ptr<MBFlowBase> m_mb_flow;

	std::shared_ptr<BytesData> m_bytes_data;
};

__codec_end