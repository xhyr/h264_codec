#pragma once

#include <cstdint>
#include <memory>

#include "global_defines.h"

__codec_begin

struct SPSData;
class BytesData;

struct SliceHeader
{
	uint32_t log2_max_frame_num_minus4;
	bool idr_pic_flag{ false };
	uint32_t log2_max_pic_order_cnt_lsb_minus4;

	uint32_t first_mb_in_slice{ 0 }; //ue(v)
	uint32_t slice_type{ 2 }; //ue(v)
	uint32_t pic_parameter_set_id{ 0 }; //ue(v)
	uint32_t frame_num{ 0 }; //u(v)
	
	//if(IdrPicFlag)
	uint32_t idr_pic_id{ 0 }; //ue(v)
	//endif(IdrPicFlag)
	
	uint32_t pic_order_cnt_lsb{ 0 };
	//if(bottom_field_pic_order_in_frame_present_flag)
	int32_t delta_pic_order_cnt_bottom{ 0 };
	//endif(bottom_field_pic_order_in_frame_present_flag)

	//dec_ref_pic_marking
	//if(IdrPicFlag)
	uint8_t no_output_of_prior_pics_flag{ 0 }; //u(1)
	uint8_t long_term_reference_flag{ 0 }; //u(1)
	//else(IdrPicFlag)
	uint8_t adaptive_ref_pic_marking_mode_flag{ 0 }; //u(1)
	//endif(IdrPicFlag)

	int32_t slice_qp_delta; //se(v)

	uint32_t disable_deblocking_filter_idc{ 1 }; //ue(v)
	int32_t alpha_c0_offset_div2{ 0 };
	int32_t beta_offset_div2{ 0 };

	void Construct(uint32_t tick, bool _idr_pic_flag, uint32_t _slice_type, std::shared_ptr<SPSData> sps_data, uint8_t qp);

	std::shared_ptr<BytesData> Convert2BytesData() const;
};

__codec_end