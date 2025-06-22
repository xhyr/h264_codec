#include "sps_data.h"

#include "bytes_data.h"
#include "coding_util.h"

__codec_begin

std::shared_ptr<BytesData> SPSData::Convert2BytesData() const
{
	auto bytes_data = std::make_shared<BytesData>();
	CodingUtil::U_V(8, profile_idc, bytes_data);
	CodingUtil::U_1(constraint_set0_flag, bytes_data);
	CodingUtil::U_1(constraint_set1_flag, bytes_data);
	CodingUtil::U_1(constraint_set2_flag, bytes_data);
	CodingUtil::U_1(constraint_set3_flag, bytes_data);
	CodingUtil::U_1(constraint_set4_flag, bytes_data);
	CodingUtil::U_1(constraint_set5_flag, bytes_data);
	CodingUtil::U_V(2, reserved_zero_2bits, bytes_data);
	CodingUtil::U_V(8, level_idc, bytes_data);
	CodingUtil::UE_V(seq_parameter_set_id, bytes_data);

	CodingUtil::UE_V(log2_max_frame_num_minus4, bytes_data);
	CodingUtil::UE_V(pic_order_cnt_type, bytes_data);
	CodingUtil::UE_V(log2_max_pic_order_cnt_lsb_minus4, bytes_data);
	CodingUtil::UE_V(max_num_ref_frames, bytes_data);
	CodingUtil::U_1(gaps_in_frame_num_value_allowed_flag, bytes_data);
	CodingUtil::UE_V(pic_width_in_mbs_minus1, bytes_data);
	CodingUtil::UE_V(pic_height_in_map_units_minus1, bytes_data);
	CodingUtil::U_1(frame_mbs_only_flag, bytes_data);
	CodingUtil::U_1(direct_8x8_inference_flag, bytes_data);
	CodingUtil::U_1(frame_cropping_flag, bytes_data);
	CodingUtil::U_1(vui_parametres_present_flag, bytes_data);

    return bytes_data;
}

__codec_end
