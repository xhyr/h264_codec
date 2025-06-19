#pragma once

#include <cstdint>

#include "global_defines.h"

__codec_begin

struct SliceHeader
{
	uint32_t first_mb_in_slice; //ue(v)
	uint32_t slice_type; //ue(v)
	uint32_t pic_parameter_set_id; //ue(v)
	uint32_t frame_num; //u(v)
	
	//if(IdrPicFlag)
	uint32_t idr_pic_id; //ue(v)
	//endif(IdrPicFlag)
	
	uint32_t pic_order_cnt_lsb;
	int32_t delta_pic_order_cnt_bottom;

	//dec_ref_pic_marking
	//if(IdrPicFlag)
	uint8_t no_output_of_prior_pics_flag; //u(1)
	uint8_t long_term_reference_flag; //u(1)
	//else(IdrPicFlag)
	uint8_t adaptive_ref_pic_marking_mode_flag; //u(1)
	//endif(IdrPicFlag)

	int32_t slice_qp_delta; //se(v)
};

__codec_end