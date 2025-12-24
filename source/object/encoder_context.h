#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "global_defines.h"

__codec_begin

struct YUVFrame;
class OStream;
class CavlcContext;
struct EncoderConfig;
class MotionInfoContext;
struct MotionVector;
class Dpb;

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
	std::shared_ptr<MotionInfoContext> motion_info_context;
	std::shared_ptr<Dpb> dpb;
	std::vector<MotionVector> search_motion_vectors;

	std::pair<int, int> mv_vertical_limit;
	std::pair<int, int> mv_horizontal_limit;

	std::shared_ptr<EncoderConfig> config;

	bool is_slice_intra;

	uint32_t mb_run{ 0 };
};

__codec_end