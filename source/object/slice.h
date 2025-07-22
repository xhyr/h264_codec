#pragma once

#include <memory>
#include <vector>

#include "slice_type.h"
#include "slice_header.h"

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

	int GetCost() const;

	std::shared_ptr<CavlcContext> GetCavlcContext();

	int GetQP() const;

	std::shared_ptr<YUVFrame> GetFrameData();

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
	std::shared_ptr<CavlcContext> m_cavlc_context;
	int m_qp;
	std::shared_ptr<BytesData> m_bytes_data;
	std::shared_ptr<YUVFrame> m_frame_data;
};

__codec_end