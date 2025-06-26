#pragma once

#include <cstdint>
#include <memory>

#include "global_defines.h"

__codec_begin

class PngPredictor
{
public:
	PngPredictor(uint32_t width, uint32_t height, std::shared_ptr<uint8_t[]> data);
	~PngPredictor();

	int Predict();

private:
	int PredictOneRow(uint32_t y) const;

	int PredictOneBlock(uint32_t begin_x, uint32_t end_x, uint32_t begin_y, uint32_t end_y) const;

	int GetLeft(uint32_t x, uint32_t y) const;
	
	int GetUp(uint32_t x, uint32_t y) const;

	int GetLeftUp(uint32_t x, uint32_t y) const;
	
	int GetNoneCost(int value) const;

	int GetSubCost(int value, int left) const;
	
	int GetUpCost(int value, int up) const;
	
	int GetAverageCost(int value, int left, int up) const;
	
	int GetPaethCost(int value, int left, int up, int left_up) const;

private:
	uint32_t m_width;
	uint32_t m_height;
	std::shared_ptr<uint8_t[]> m_data;
};

__codec_end