#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "global_defines.h"

__codec_begin

class Macroblock;
struct EncoderContext;

struct MBNeighbors
{
public:
	MBNeighbors(std::weak_ptr<Macroblock> macroblock, uint32_t mb_addr, std::shared_ptr<EncoderContext> encoder_context);

	void ObtainLeftAndUpData(std::vector<uint8_t>& left_data, std::vector<uint8_t>& up_data);

private:
	void Init(std::shared_ptr<EncoderContext> encoder_context);

private:
	std::weak_ptr<Macroblock> m_macroblock;
	uint32_t m_e_addr;

	bool m_a_available;
	bool m_b_available;
	bool m_c_available;
	bool m_d_available;
	uint32_t m_addr_a;
	uint32_t m_addr_b;
	uint32_t m_addr_c;
	uint32_t m_addr_d;
};

__codec_end