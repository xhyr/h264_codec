#pragma once

#include <tuple>

#include "block_data.h"

__codec_begin

struct QuantizeUtil
{
	static BlockData<4, 4, int> QuantizeDC(int qp, const BlockData<4, 4, int>& dc_block);

	static BlockData<4, 4, int> InverseQuantizeDC(int qp, const BlockData<4, 4, int>& dc_block);

	static BlockData<4, 4, int> QuantizeAC(int qp, const BlockData<4, 4, int>& ac_block);

	static BlockData<4, 4, int> InverseQuantizeAC(int qp, const BlockData<4, 4, int>& ac_block);

private:
	static std::tuple<int, int, int> GetQuantizeParameters(int qp, bool is_intra = true);

	static std::pair<int, int> GetInverseQuantizeParameters(int qp);
};

__codec_end