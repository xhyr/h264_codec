#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "common_types.h"
#include "block_data.h"
#include "cavlc_types.h"
#include "mb_type.h"

__codec_begin

class Macroblock;
struct EncoderContext;
class BytesData;
class ChromaQuantizer;

class InterChromaFlowBase
{
public:
	InterChromaFlowBase(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	virtual ~InterChromaFlowBase();

	virtual void Frontend() = 0;

	virtual uint32_t OutputCoefficients(std::shared_ptr<BytesData> bytes_data) = 0;

	BlockData<8, 8> GetReconstructedData(PlaneType plane_type);

	uint8_t GetCBP() const;

	int GetDistortion() const;

protected:
	std::shared_ptr<Macroblock> m_mb;
	std::shared_ptr<EncoderContext> m_encoder_context;
	std::unique_ptr<ChromaQuantizer> m_quantizer;

	std::unordered_map<PlaneType, BlockData<8, 8>> m_reconstructed_data_map;
	uint8_t m_cbp{ 0 };
	std::vector<BlockData<4, 4, int32_t>> m_diff_blocks;
	CavlcDataSource m_cavlc_data_source;
	int m_distortion{ 0 };

public:
	static std::shared_ptr<InterChromaFlowBase> CreateFlow(MBType mb_type, std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);

};

__codec_end