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

class Slice : public std::enable_shared_from_this<Slice>
{
public:
	SliceType GetType() const;

	void Construct(SliceType slice_type, std::shared_ptr<SPS> sps, std::shared_ptr<PPS> pps);

	bool Encode(std::shared_ptr<EncoderContext> encoder_context);

	void Serial(std::shared_ptr<OStream> ostream);

	std::shared_ptr<Macroblock> GetMacroblock(uint32_t mb_addr);

	int GetCost() const;

	std::shared_ptr<CavlcContext> GetCavlcContext();

private:
	SliceType m_type;
	SliceHeader m_header;
	std::vector<std::shared_ptr<Macroblock>> m_macroblocks;
	int m_cost{ 0 };
	std::shared_ptr<CavlcContext> m_cavlc_context;
};

__codec_end