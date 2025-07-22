#include "block_neighbors.h"

#include "macroblock.h"
#include "slice.h"
#include "yuv_frame.h"

__codec_begin

BlockNeighbors::BlockNeighbors(std::shared_ptr<Macroblock> mb, uint32_t x_in_block, uint32_t y_in_block) :
	m_mb(mb), m_x_in_block(x_in_block), m_y_in_block(y_in_block)
{
	Init();
}

BlockNeighbors::~BlockNeighbors()
{
}

NeighborEdgeData BlockNeighbors::GetEdgeData() const
{
	return m_edge_data;
}

bool BlockNeighbors::IsUpAvailable() const
{
	return m_is_up_available;
}

bool BlockNeighbors::IsLeftAvailable() const
{
	return m_is_left_available;
}

bool BlockNeighbors::IsUpLeftAvaiable() const
{
	return m_is_up_available && m_is_left_available;
}

void BlockNeighbors::Init()
{
	auto yuv_frame = m_mb->GetSlice()->GetFrameData();
	auto luma_data = yuv_frame->y_data;
	auto width = yuv_frame->width;
	auto [x, y] = m_mb->GetPosition();
	x += m_x_in_block * 4;
	y += m_y_in_block * 4;

	auto luma_data_ptr = luma_data.get();

	if (y > 0)
	{
		m_is_up_available = true;
		m_edge_data.a = luma_data_ptr[(y - 1) * width + x + 0];
		m_edge_data.b = luma_data_ptr[(y - 1) * width + x + 1];
		m_edge_data.c = luma_data_ptr[(y - 1) * width + x + 2];
		m_edge_data.d = luma_data_ptr[(y - 1) * width + x + 3];
	}

	if (x > 0)
	{
		m_is_left_available = true;
		m_edge_data.i = luma_data_ptr[(y + 0) * width + x - 1];
		m_edge_data.j = luma_data_ptr[(y + 1) * width + x - 1];
		m_edge_data.k = luma_data_ptr[(y + 2) * width + x - 1];
		m_edge_data.l = luma_data_ptr[(y + 3) * width + x - 1];
	}

	if (m_is_left_available && m_is_up_available)
		m_edge_data.x = luma_data_ptr[(y - 1) * width + x - 1];

	if (m_is_up_available)
	{
		if (x + 4 < width && !(m_x_in_block == 1 && m_y_in_block == 1) && !(m_x_in_block == 1 && m_y_in_block == 3))
		{
			m_edge_data.e = luma_data_ptr[(y - 1) * width + x + 4];
			m_edge_data.f = luma_data_ptr[(y - 1) * width + x + 5];
			m_edge_data.g = luma_data_ptr[(y - 1) * width + x + 6];
			m_edge_data.h = luma_data_ptr[(y - 1) * width + x + 7];
		}
		else
		{
			m_edge_data.e = m_edge_data.f = m_edge_data.g = m_edge_data.h = m_edge_data.d;
		}
	}
}

__codec_end