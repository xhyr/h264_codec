#pragma once

#include <cstdint>
#include <memory>

#include "global_defines.h"

__codec_begin

class BytesData;

struct PPSData
{
	uint32_t pic_parameter_set_id{ 0 }; //ue(v)
	uint32_t seq_parameter_set_id{ 0 }; //ue(v)
	uint8_t entropy_coding_mode_flag{ 0 }; //u(1)
	uint8_t bottom_field_pic_order_in_frame_present_flag{ 0 }; //u(1)
	uint32_t num_slice_groups_minus1{ 0 }; //ue(v)
	uint32_t num_ref_idx_l0_default_active_minus1; //ue(v)
	uint32_t num_ref_idx_l1_default_active_minus1; //ue(v)
	uint8_t weighted_pred_flag{ 0 }; //u(1)
	uint8_t weighted_bipred_idc{ 0 }; //u(2)
	int32_t pic_init_qp_minus_26{ 0 }; //se(v)
	int32_t pic_init_qs_minus_26{ 0 }; //se(v)
	int32_t chroma_qp_index_offset{ 0 }; //se(v)
	uint8_t deblocking_filter_control_present_flag{ 1 }; //u(1)
	uint8_t constrained_intra_pred_flag{ 0 }; //u(1)
	uint8_t redundant_pic_cnt_present_flag{ 0 }; //u(1)

	std::shared_ptr<BytesData> Convert2BytesData() const;

};

__codec_end