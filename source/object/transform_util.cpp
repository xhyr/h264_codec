#include "transform_util.h"

__codec_begin

BlockData<4, 4, int> TransformUtil::Hadamard(const BlockData<4, 4, int>& block_data, int divide_factor)
{
	BlockData<4, 4, int> intermediate_block;
	//right matrix mutiple, horizontal
	for (uint32_t y = 0; y < 4; ++y)
	{
		auto p0 = block_data.GetElement(0, y);
		auto p1 = block_data.GetElement(1, y);
		auto p2 = block_data.GetElement(2, y);
		auto p3 = block_data.GetElement(3, y);
	
		auto t0 = p0 + p3;
		auto t1 = p1 + p2;
		auto t2 = p1 - p2;
		auto t3 = p0 - p3;

		intermediate_block.SetElement(0, y, t0 + t1);
		intermediate_block.SetElement(1, y, t2 + t3);
		intermediate_block.SetElement(2, y, t0 - t1);
		intermediate_block.SetElement(3, y, t3 - t2);
	}

	BlockData<4, 4, int> output_block;
	//left matrix mutiple, vertical
	for (uint32_t x = 0; x < 4; ++x)
	{
		auto p0 = intermediate_block.GetElement(x, 0);
		auto p1 = intermediate_block.GetElement(x, 1);
		auto p2 = intermediate_block.GetElement(x, 2);
		auto p3 = intermediate_block.GetElement(x, 3);

		auto t0 = p0 + p3;
		auto t1 = p1 + p2;
		auto t2 = p1 - p2;
		auto t3 = p0 - p3;

		output_block.SetElement(x, 0, (t0 + t1) / divide_factor);
		output_block.SetElement(x, 1, (t2 + t3) / divide_factor);
		output_block.SetElement(x, 2, (t0 - t1) / divide_factor);
		output_block.SetElement(x, 3, (t3 - t2) / divide_factor);
	}

	return output_block;
}

BlockData<4, 4, int> TransformUtil::InverseHadamard(const BlockData<4, 4, int>& block_data)
{
	return Hadamard(block_data, 1);
}

BlockData<4, 4, int> TransformUtil::DCT(const BlockData<4, 4, int>& block_data)
{
	BlockData<4, 4, int> intermediate_block;
	//right matrix mutiple, horizontal
	for (uint32_t y = 0; y < 4; ++y)
	{
		auto p0 = block_data.GetElement(0, y);
		auto p1 = block_data.GetElement(1, y);
		auto p2 = block_data.GetElement(2, y);
		auto p3 = block_data.GetElement(3, y);

		auto t0 = p0 + p3;
		auto t1 = p1 + p2;
		auto t2 = p1 - p2;
		auto t3 = p0 - p3;

		intermediate_block.SetElement(0, y, t0 + t1);
		intermediate_block.SetElement(1, y, (t3 << 1) + t2);
		intermediate_block.SetElement(2, y, t0 - t1);
		intermediate_block.SetElement(3, y, t3 - (t2 << 1));
	}

	BlockData<4, 4, int> output_block;
	//left matrix mutiple, vertical
	for (uint32_t x = 0; x < 4; ++x)
	{
		auto p0 = intermediate_block.GetElement(x, 0);
		auto p1 = intermediate_block.GetElement(x, 1);
		auto p2 = intermediate_block.GetElement(x, 2);
		auto p3 = intermediate_block.GetElement(x, 3);
	
		auto t0 = p0 + p3;
		auto t1 = p1 + p2;
		auto t2 = p1 - p2;
		auto t3 = p0 - p3;

		output_block.SetElement(x, 0, t0 + t1);
		output_block.SetElement(x, 1, t2 + (t3 << 1));
		output_block.SetElement(x, 2, t0 - t1);
		output_block.SetElement(x, 3, t3 - (t2 << 1));
	}

	return output_block;
}

BlockData<4, 4, int> TransformUtil::InverseDCT(const BlockData<4, 4, int>& block_data)
{
	BlockData<4, 4, int> intermediate_block;
	//right matrix mutiple, horizontal
	for (uint32_t y = 0; y < 4; ++y)
	{
		auto p0 = block_data.GetElement(0, y);
		auto p1 = block_data.GetElement(1, y);
		auto p2 = block_data.GetElement(2, y);
		auto p3 = block_data.GetElement(3, y);

		auto t0 = p0 + p2;
		auto t1 = p0 - p2;
		auto t2 = (p1 >> 1) - p3;
		auto t3 = p1 + (p3 >> 1);

		intermediate_block.SetElement(0, y, t0 + t3);
		intermediate_block.SetElement(1, y, t1 + t2);
		intermediate_block.SetElement(2, y, t1 - t2);
		intermediate_block.SetElement(3, y, t0 - t3);
	}

	BlockData<4, 4, int> output_block;
	//left matrix mutiple, vertical
	for (uint32_t x = 0; x < 4; ++x)
	{
		auto p0 = intermediate_block.GetElement(x, 0);
		auto p1 = intermediate_block.GetElement(x, 1);
		auto p2 = intermediate_block.GetElement(x, 2);
		auto p3 = intermediate_block.GetElement(x, 3);
	
		auto t0 = p0 + p2;
		auto t1 = p0 - p2;
		auto t2 = (p1 >> 1) - p3;
		auto t3 = p1 + (p3 >> 1);

		output_block.SetElement(x, 0, t0 + t3);
		output_block.SetElement(x, 1, t1 + t2);
		output_block.SetElement(x, 2, t1 - t2);
		output_block.SetElement(x, 3, t0 - t3);
	}

	return output_block;
}

__codec_end
