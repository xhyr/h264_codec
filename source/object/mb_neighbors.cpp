#include "mb_neighbors.h"

#include "macroblock.h"
#include "encoder_context.h"
#include "mb_util.h"

__codec_begin

MBNeighbors::MBNeighbors(std::weak_ptr<Macroblock> macroblock, uint32_t mb_addr, std::shared_ptr<EncoderContext> encoder_context) : 
	m_macroblock(macroblock), m_e_addr(mb_addr)
{
	Init(encoder_context);
}

void MBNeighbors::ObtainLeftAndUpEdge(std::vector<uint8_t>& left_data, std::vector<uint8_t>& up_data, uint8_t& left_up_element, PlaneType plane_type)
{
	if (m_a_available)
	{
		auto mb = m_macroblock.lock();
		left_data = mb->GetMacroblock(m_addr_a)->GetRightData(plane_type);
	}

	if (m_b_available)
	{
		auto mb = m_macroblock.lock();
		up_data = mb->GetMacroblock(m_addr_b)->GetDownData(plane_type);
	}

	if (m_d_available)
	{
		auto mb = m_macroblock.lock();
		left_up_element = mb->GetMacroblock(m_addr_d)->GetDownData(plane_type).back();
	}
}

void MBNeighbors::Init(std::shared_ptr<EncoderContext> encoder_context)
{
	m_addr_a = m_e_addr - 1;
	m_addr_b = m_e_addr - encoder_context->width_in_mb;
	m_addr_c = m_e_addr - encoder_context->width_in_mb + 1;
	m_addr_d = m_e_addr - encoder_context->width_in_mb - 1;

	m_a_available = MBUtil::IsAvailable(m_addr_a, encoder_context) && ((m_e_addr % encoder_context->width_in_mb) != 0);
	m_b_available = MBUtil::IsAvailable(m_addr_b, encoder_context);
	m_c_available = MBUtil::IsAvailable(m_addr_c, encoder_context) && (((m_e_addr + 1) % encoder_context->width_in_mb) != 0);
	m_d_available = MBUtil::IsAvailable(m_addr_d, encoder_context) && ((m_e_addr % encoder_context->width_in_mb) != 0);
}	

__codec_end


