#pragma once

#include <memory>
#include <vector>

#include "slice_type.h"
#include "slice_header.h"
#include "block_data.h"

__codec_begin

class SPS;
class PPS;
struct EncoderContext;
class OStream;
class Macroblock;
class CavlcContext;
struct YUVFrame;

class Slice : public std::enable_shared_from_this<Slice>
{
public:
	SliceType GetType() const;

	void Construct(uint32_t tick, SliceType slice_type, std::shared_ptr<SPS> sps, std::shared_ptr<PPS> pps, std::shared_ptr<EncoderContext> encoder_context);

	bool Encode();

	void Serial(std::shared_ptr<OStream> ostream);

	std::shared_ptr<Macroblock> GetMacroblock(uint32_t mb_addr);

	int GetQP() const;

	std::shared_ptr<YUVFrame> GetFrameData();

	void RefreshBlockData(uint32_t mb_addr, uint32_t x_in_block, uint32_t y_in_block, const BlockData<4, 4>& block_data);

private:
	void DeblockFilter();

	void CollectFrameData(std::shared_ptr<Macroblock> mb);

	void CollectAllFrameData();

private:
	uint32_t m_tick;
	SliceType m_type;
	std::shared_ptr<EncoderContext> m_encoder_context;
	SliceHeader m_header;
	std::vector<std::shared_ptr<Macroblock>> m_macroblocks;
	int m_cost{ 0 };
	int m_qp;
	std::shared_ptr<BytesData> m_bytes_data;
	std::shared_ptr<YUVFrame> m_frame_data;
};

__codec_end