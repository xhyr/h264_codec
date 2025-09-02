#pragma once

#include <cstdint>
#include <memory>

#include "global_defines.h"

__codec_begin

struct YUVFrame;
class OStream;
class CavlcContext;
struct EncoderConfig;

struct EncoderContext
{
	uint32_t width;
	uint32_t height;
	uint32_t width_in_mb;
	uint32_t height_in_mb;
	uint32_t mb_num;
	std::shared_ptr<YUVFrame> yuv_frame;
	std::shared_ptr<OStream> out_stream;

	int slice_addr;
	int mb_addr;
	int qp;
	double lambda_mode;
	double lambda_motion;
	int lambda_mode_fp;
	int lambda_motion_fp;
	std::shared_ptr<CavlcContext> cavlc_context;
	std::shared_ptr<YUVFrame> last_frame;

	std::shared_ptr<EncoderConfig> config;
};

__codec_end