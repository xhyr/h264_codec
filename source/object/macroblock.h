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

private:
	void Init();

	void ComputePosition();

	void ObtainData();

	void PreEncode();

	void DoEncode();

	void PostEncode();

	void IntraPredict();

private:
	uint32_t m_addr;
	std::weak_ptr<Slice> m_slice;
	std::shared_ptr<EncoderContext> m_encoder_context;

	Position m_pos_in_mb;
	Position m_pos;
	MBNeighbors m_neighbors;
	uint32_t m_qp;
	MBType m_type;

	BlockData<16, 16> m_luma_data;
};

__codec_end