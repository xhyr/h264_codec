#pragma once

#include <memory>

#include "global_defines.h"

__codec_begin

class Macroblock;
struct EncoderContext;
class BytesData;

class MBInterRDOptimizer final
{
public:
	MBInterRDOptimizer(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context);
	~MBInterRDOptimizer();

	void Encode();

	void Binary(std::shared_ptr<BytesData> bytes_data);

};

__codec_end