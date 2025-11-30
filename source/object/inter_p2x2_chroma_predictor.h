#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include "mb_type.h"
#include "block_data.h"
#include "common_types.h"

__codec_begin

class Macroblock;
struct EncoderContext;

class InterP2x2ChromaPredictor
{
public:
	InterP2x2ChromaPredictor(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context, uint8_t segment_index, uint8_t sub_segment_index, bool skip);
	~InterP2x2ChromaPredictor();

	void Decide();

	void FillDiffData(PlaneType plane_type, std::vector<BlockData<4, 4, int32_t>>& diff_datas);

	void FillPredictedData(PlaneType plane_type, BlockData<8, 8>& predicted_data);

private:
	BlockData<2, 2> GetPredictedData(PlaneType plane_type) const;

	BlockData<2, 2, int32_t> GetDiffData(PlaneType plane_type) const;

private:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	uint8_t m_segment_index;
	uint8_t m_sub_segment_index;
	bool m_skip;

	BlockData<2, 2> m_predicted_data[2];
	BlockData<2, 2, int32_t> m_diff_data[2];
};

__codec_end