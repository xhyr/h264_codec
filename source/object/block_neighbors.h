#pragma once

#include <cstdint>
#include <memory>

#include "block_data.h"
#include "mb_type.h"
#include "prediction_type.h"

__codec_begin

class Macroblock;
struct NeighborEdgeData;
struct YUVFrame;

class BlockNeighbors
{
public:
	BlockNeighbors(std::shared_ptr<Macroblock> mb, uint32_t x_in_block, uint32_t y_in_block, const BlockData<16, 16>& reconstructed_block_data, const std::vector<Intra4LumaPredictionType>& prediction_types);
	~BlockNeighbors();

	std::shared_ptr<NeighborEdgeData> GetEdgeData() const;

	bool IsUpAvailable() const;

	bool IsLeftAvailable() const;

	bool IsUpLeftAvaiable() const;

	MBType GetUpMBType() const;

	MBType GetLeftMBType() const;

	Intra4LumaPredictionType GetUpPredictionType() const;

	Intra4LumaPredictionType GetLeftPredictionType() const;

private:
	void Init();

	uint8_t GetPixelValue(uint32_t x, uint32_t y);
	
private:
	std::shared_ptr<Macroblock> m_mb;
	uint32_t m_x_in_block;
	uint32_t m_y_in_block;
	const BlockData<16, 16>& m_reconstructed_block_data;
	const std::vector<Intra4LumaPredictionType>& m_prediction_types;

	std::shared_ptr<NeighborEdgeData> m_edge_data;
	bool m_is_up_available{ false };
	bool m_is_left_available{ false };

	uint32_t m_mb_x;
	uint32_t m_mb_y;
	uint32_t m_slice_width;
	std::shared_ptr<YUVFrame> m_slice_frame;
};


__codec_end