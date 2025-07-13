#include "slice_header.h"

#include "bytes_data.h"
#include "coding_util.h"
#include "sps_data.h"

__codec_begin

void SliceHeader::Construct(uint32_t tick, bool _idr_pic_flag, uint32_t _slice_type, std::shared_ptr<SPSData> sps_data)
{
	frame_num = tick;
	pic_order_cnt_lsb = tick * 2;

	log2_max_frame_num_minus4 = sps_data->log2_max_frame_num_minus4;
	idr_pic_flag = _idr_pic_flag;
	log2_max_pic_order_cnt_lsb_minus4 = sps_data->log2_max_pic_order_cnt_lsb_minus4;
	
	slice_type = _slice_type;
}

std::shared_ptr<BytesData> SliceHeader::Convert2BytesData() const
{
	auto bytes_data = std::make_shared<BytesData>();
	CodingUtil::UE_V(first_mb_in_slice, bytes_data);
	CodingUtil::UE_V(slice_type, bytes_data);
	CodingUtil::UE_V(pic_parameter_set_id, bytes_data);
	CodingUtil::U_V(log2_max_frame_num_minus4 + 4, frame_num, bytes_data);
	if (idr_pic_flag)
		CodingUtil::UE_V(idr_pic_id, bytes_data);
	CodingUtil::U_V(log2_max_pic_order_cnt_lsb_minus4 + 4, pic_order_cnt_lsb, bytes_data);

	if (idr_pic_flag)
	{
		CodingUtil::U_1(no_output_of_prior_pics_flag, bytes_data);
		CodingUtil::U_1(long_term_reference_flag, bytes_data);
	}
	else
	{
		CodingUtil::U_1(adaptive_ref_pic_marking_mode_flag, bytes_data);
	}

	CodingUtil::SE_V(slice_qp_delta, bytes_data);
	CodingUtil::UE_V(disable_deblocking_filter_idc, bytes_data);
	return bytes_data;
}

__codec_end