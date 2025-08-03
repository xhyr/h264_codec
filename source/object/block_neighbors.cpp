#include "block_neighbors.h"

#include "neighbor_edge_data.h"
#include "macroblock.h"
#include "slice.h"
#include "yuv_frame.h"

__codec_begin

BlockNeighbors::BlockNeighbors(std::shared_ptr<Macroblock> mb, uint32_t x_in_block, uint32_t y_in_block, const BlockData<16, 16>& reconstructed_block_data, const std::vector<Intra4LumaPredictionType>& prediction_types) :
	m_mb(mb), m_x_in_block(x_in_block), m_y_in_block(y_in_block), m_reconstructed_block_data(reconstructed_block_data), 
	m_prediction_types(prediction_types), m_edge_data(std::make_shared<NeighborEdgeData>())
{
	Init();
}

BlockNeighbors::~BlockNeighbors()
{
}

std::shared_ptr<NeighborEdgeData> BlockNeighbors::GetEdgeData() const
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

MBType BlockNeighbors::GetUpMBType() const
{
	return m_y_in_block == 0 ? (m_mb->GetUpMacroblock()->GetType()) : MBType::I4;
}

MBType BlockNeighbors::GetLeftMBType() const
{
	return m_x_in_block == 0 ? (m_mb->GetLeftMacroblock()->GetType()) : MBType::I4;
}

Intra4LumaPredictionType BlockNeighbors::GetUpPredictionType() const
{
	if (m_y_in_block == 0)
	{
		auto up_mb = m_mb->GetUpMacroblock();
		return up_mb->GetIntra4LumaPredicionType(m_x_in_block, 3);
	}
	else return m_prediction_types[m_x_in_block + 4 * (m_y_in_block - 1)];
}

Intra4LumaPredictionType BlockNeighbors::GetLeftPredictionType() const
{
	if (m_x_in_block == 0)
	{
		auto left_mb = m_mb->GetLeftMacroblock();
		return left_mb->GetIntra4LumaPredicionType(3, m_y_in_block);
	}
	else return m_prediction_types[m_x_in_block - 1 + 4 * m_y_in_block];
}

void BlockNeighbors::Init()
{
	std::tie(m_mb_x, m_mb_y) = m_mb->GetPosition();
	auto x = m_mb_x + m_x_in_block * 4;
	auto y=  m_mb_y + m_y_in_block * 4;
	m_slice_frame = m_mb->GetSlice()->GetFrameData();
	m_slice_width = m_slice_frame->width;

	if (y > 0)
	{
		m_is_up_available = true;
		m_edge_data->a = GetPixelValue(x + 0, y - 1);
		m_edge_data->b = GetPixelValue(x + 1, y - 1);
		m_edge_data->c = GetPixelValue(x + 2, y - 1);
		m_edge_data->d = GetPixelValue(x + 3, y - 1);
	}

	if (x > 0)
	{
		m_is_left_available = true;
		m_edge_data->i = GetPixelValue(x - 1, y + 0);
		m_edge_data->j = GetPixelValue(x - 1, y + 1);
		m_edge_data->k = GetPixelValue(x - 1, y + 2);
		m_edge_data->l = GetPixelValue(x - 1, y + 3);
	}

	if (m_is_left_available && m_is_up_available)
		m_edge_data->m = GetPixelValue(x - 1, y - 1);

	if (m_is_up_available)
	{
		if (x + 4 < m_slice_width && !(m_x_in_block == 1 && m_y_in_block == 1) && !(m_x_in_block == 1 && m_y_in_block == 3))
		{
			m_edge_data->e = GetPixelValue(x + 4, y -1);
			m_edge_data->f = GetPixelValue(x + 5, y -1);
			m_edge_data->g = GetPixelValue(x + 6, y -1);
			m_edge_data->h = GetPixelValue(x + 7, y -1);
		}
		else
		{
			m_edge_data->e = m_edge_data->f = m_edge_data->g = m_edge_data->h = m_edge_data->d;
		}
	}
}

uint8_t BlockNeighbors::GetPixelValue(uint32_t x, uint32_t y)
{
	if (x >= m_mb_x && x < m_mb_x + 16 && y >= m_mb_y && y < m_mb_y + 16)
		return m_reconstructed_block_data.GetElement(x - m_mb_x, y - m_mb_y);
	else
	{
		auto luma_data_ptr = m_slice_frame->y_data.get();
		return luma_data_ptr[y * m_slice_width + x];
	}
}

__codec_end