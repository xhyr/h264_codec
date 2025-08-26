#pragma once

#include <cstdint>
#include <memory>

#include "global_defines.h"

__codec_begin

class BytesData;

struct SPSData
{
	uint8_t profile_idc; //u(8), 66 = baseline, 77 = main
	uint8_t constraint_set0_flag{ 0 }; //u(1)
	uint8_t constraint_set1_flag{ 0 }; //u(1)
	uint8_t constraint_set2_flag{ 0 }; //u(1)
	uint8_t constraint_set3_flag{ 0 }; //u(1)
	uint8_t constraint_set4_flag{ 0 }; //u(1)
	uint8_t constraint_set5_flag{ 0 }; //u(1)
	uint8_t reserved_zero_2bits{ 0 }; //u(2), equal to 0
	uint8_t level_idc; //u(8}
	uint32_t seq_parameter_set_id{ 0 }; //ue(v)

	uint32_t log2_max_frame_num_minus4{ 0 }; //ue(v)
	uint32_t pic_order_cnt_type{ 0 }; //ue(v)
	uint32_t log2_max_pic_order_cnt_lsb_minus4{ 0 }; //ue(v)
	uint32_t max_num_ref_frames; //ue(v)
	uint8_t gaps_in_frame_num_value_allowed_flag{ 0 }; //u(1)
	uint32_t pic_width_in_mbs_minus1; //ue(v)
	uint32_t pic_height_in_map_units_minus1; //ue(v)
	uint8_t frame_mbs_only_flag{ 1 }; //u(1)
	uint8_t direct_8x8_inference_flag{ 1 }; //u(0)
	uint8_t frame_cropping_flag{ 0 }; //u(1)
	uint8_t vui_parametres_present_flag{ 0 }; //u(1)

	std::shared_ptr<BytesData> Convert2BytesData() const;

};



__codec_end