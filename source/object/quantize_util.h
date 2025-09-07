#pragma once

#include <tuple>

#include "block_data.h"

__codec_begin

struct QuantizeUtil
{
	static BlockData<4, 4, int32_t> QuantizeDC(int qp, const BlockData<4, 4, int32_t>& dc_block);

	static BlockData<2, 2, int32_t> QuantizeDC(int qp, const BlockData<2, 2, int32_t>& dc_block, bool is_intra = true);

	static BlockData<4, 4, int32_t> InverseQuantizeDC(int qp, const BlockData<4, 4, int32_t>& dc_block);

	static BlockData<2, 2, int32_t> InverseQuantizeDC(int qp, const BlockData<2, 2, int32_t>& dc_block);

	static BlockData<4, 4, int32_t> QuantizeAC(int qp, const BlockData<4, 4, int32_t>& ac_block, bool is_intra = true);

	static BlockData<4, 4, int32_t> InverseQuantizeAC(int qp, const BlockData<4, 4, int32_t>& ac_block);

	static BlockData<4, 4, int32_t> QuantizeNormal(int qp, const BlockData<4, 4, int32_t>& block, bool is_intra = true);

	static BlockData<4, 4, int32_t> InverseQuantizeNormal(int qp, const BlockData<4, 4, int32_t>& block);

private:
	static std::tuple<int, int, int> GetQuantizeParameters(int qp, bool is_intra = true);

	static std::pair<int, int> GetInverseQuantizeParameters(int qp);
};

__codec_end