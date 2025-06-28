#include "transformer.h"

__codec_begin

BlockData<4, 4, int> Transformer::Hadamard(const BlockData<4, 4, int>& block_data, int divide_factor)
{
	BlockData<4, 4, int> intermediate;
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

		intermediate.SetElement(0, y, t0 + t1);
		intermediate.SetElement(1, y, t2 + t3);
		intermediate.SetElement(2, y, t0 - t1);
		intermediate.SetElement(3, y, t3 - t2);
	}

	BlockData<4, 4, int> output;
	//left matrix mutiple, vertical
	for (uint32_t x = 0; x < 4; ++x)
	{
		auto p0 = intermediate.GetElement(x, 0);
		auto p1 = intermediate.GetElement(x, 1);
		auto p2 = intermediate.GetElement(x, 2);
		auto p3 = intermediate.GetElement(x, 3);

		auto t0 = p0 + p3;
		auto t1 = p1 + p2;
		auto t2 = p1 - p2;
		auto t3 = p0 - p3;

		output.SetElement(x, 0, (t0 + t1) / divide_factor);
		output.SetElement(x, 1, (t2 + t3) / divide_factor);
		output.SetElement(x, 2, (t0 - t1) / divide_factor);
		output.SetElement(x, 3, (t3 - t2) / divide_factor);
	}

	return output;
}

BlockData<4, 4, int> Transformer::InverseHadamard(const BlockData<4, 4, int>& block_data)
{
	return Hadamard(block_data, 1);
}

BlockData<4, 4, int> Transformer::DCT(const BlockData<4, 4, int>& block_data)
{
	BlockData<4, 4, int> intermediate;
	//right matrix mutiple, horizontal
	for (uint32_t y = 0; y < 4; ++y)
	{
		auto p0 = block_data.GetElement(0, y);
		auto p1 = block_data.GetElement(1, y);
		auto p2 = block_data.GetElement(2, y);
		auto p3 = block_data.GetElement(3, y);



	}

	BlockData<4, 4, int> output;
	//left matrix mutiple, vertical

	return output;
}

BlockData<4, 4, int> Transformer::InverseDCT(const BlockData<4, 4, int>& block_data)
{
	
}

__codec_end
