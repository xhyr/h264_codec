#include "pps_data.h"

#include "bytes_data.h"
#include "coding_util.h"

__codec_begin

std::shared_ptr<BytesData> PPSData::Convert2BytesData() const
{
	auto bytes_data = std::make_shared<BytesData>();
	CodingUtil::UE_V(pic_parameter_set_id, bytes_data);
	CodingUtil::UE_V(seq_parameter_set_id, bytes_data);
	CodingUtil::U_1(entropy_coding_mode_flag, bytes_data);
	CodingUtil::U_1(bottom_field_pic_order_in_frame_present_flag, bytes_data);
	CodingUtil::UE_V(num_slice_groups_minus1, bytes_data);
	CodingUtil::UE_V(num_ref_idx_l0_default_active_minus1, bytes_data);
	CodingUtil::UE_V(num_ref_idx_l1_default_active_minus1, bytes_data);
	CodingUtil::U_1(weighted_pred_flag, bytes_data);
	CodingUtil::U_1(weighted_bipred_idc, bytes_data);
	CodingUtil::SE_V(pic_init_qp_minus_26, bytes_data);
	CodingUtil::SE_V(pic_init_qs_minus_26, bytes_data);
	CodingUtil::SE_V(chroma_qp_index_offset, bytes_data);
	CodingUtil::U_1(deblocking_filter_control_present_flag, bytes_data);
	CodingUtil::U_1(constrained_intra_pred_flag, bytes_data);
	CodingUtil::U_1(redundant_pic_cnt_present_flag, bytes_data);
	return bytes_data;
}

__codec_end
