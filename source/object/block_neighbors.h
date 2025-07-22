#pragma once

#include <cstdint>
#include <memory>

#include "neighbor_edge_data.h"

__codec_begin

class Macroblock;

class BlockNeighbors
{
public:
	BlockNeighbors(std::shared_ptr<Macroblock> mb, uint32_t x_in_block, uint32_t y_in_block);
	~BlockNeighbors();

	NeighborEdgeData GetEdgeData() const;

	bool IsUpAvailable() const;

	bool IsLeftAvailable() const;

	bool IsUpLeftAvaiable() const;

private:
	void Init();
	
private:
	std::shared_ptr<Macroblock> m_mb;
	uint32_t m_x_in_block;
	uint32_t m_y_in_block;

	NeighborEdgeData m_edge_data;
	bool m_is_up_available{ false };
	bool m_is_left_available{ false };
};


__codec_end