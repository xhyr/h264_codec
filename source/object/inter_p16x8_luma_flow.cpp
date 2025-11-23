#include "inter_p16x8_luma_flow.h"

#include "coding_util.h"
#include "bytes_data.h"
#include "inter_p16x8_luma_predictor.h"
#include "encoder_context.h"
#include "segment_util.h"
#include "reconstruct_util.h"

__codec_begin

InterP16x8LumaFlow::InterP16x8LumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	InterLumaFlowBase(mb, encoder_context)
{
}

InterP16x8LumaFlow::~InterP16x8LumaFlow()
{
}

void InterP16x8LumaFlow::Frontend()
{
	Predict();
	TransformAndQuantize();
	InverseQuantizeAndTransform();
	Reconstruct();

	CalculateDistortion();
}

void InterP16x8LumaFlow::Backend()
{
	for (auto& predictor : m_predictors)
		predictor->UpdateMotionInfo();
}

uint32_t InterP16x8LumaFlow::OutputMotionInfo(std::shared_ptr<BytesData> bytes_data) const
{
	auto start_bit_count = bytes_data->GetBitsCount();

	for (uint8_t segment_index = 0; segment_index < 2; ++segment_index)
	{
		auto mvd = m_predictors[segment_index]->GetMVD();

		CodingUtil::SE_V(mvd.x, bytes_data);
		CodingUtil::SE_V(mvd.y, bytes_data);
	}

	auto finish_bit_count = bytes_data->GetBitsCount();
	return finish_bit_count - start_bit_count;
}

void InterP16x8LumaFlow::Predict()
{
	for (uint8_t segment_index = 0; segment_index < 2; ++segment_index)
	{
		m_predictors[segment_index] = std::make_unique<InterP16x8LumaPredictor>(m_mb, m_encoder_context, segment_index);
		m_predictors[segment_index]->Decide();
		m_predictors[segment_index]->FillDiffData(m_diff_datas);
		m_predictors[segment_index]->UpdateMotionInfo();
	}
}

void InterP16x8LumaFlow::Reconstruct()
{
	for (uint8_t segment_index = 0; segment_index < 2; ++segment_index)
	{
		auto predicted_data = m_predictors[segment_index]->GetPredictedData();
		auto [start_x, start_y] = SegmentUtil::GetLumaDataRect<16, 8>(segment_index);
		for (uint32_t y = start_y; y < start_y + 8; ++y)
		{
			for (uint32_t x = start_x; x < start_x + 16; ++x)
			{
				auto predicted_val = predicted_data.GetElement(x - start_x, y - start_y);
				auto residual_val = m_cbp == 0 ? 0 : m_diff_data.GetElement(x, y);
				auto val = ReconstructUtil::Reconstruct(predicted_val, residual_val);
				m_reconstructed_data.SetElement(x, y, val);
			}
		}
	}
}

__codec_end