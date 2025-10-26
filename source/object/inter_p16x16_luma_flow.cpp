#include "inter_p16x16_luma_flow.h"

#include "coding_util.h"
#include "bytes_data.h"
#include "inter_p16x16_luma_predictor.h"
#include "encoder_context.h"
#include "motion_info_context.h"
#include "reconstruct_util.h"

__codec_begin

InterP16x16LumaFlow::InterP16x16LumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	InterLumaFlowBase(mb, encoder_context)
{
}

InterP16x16LumaFlow::~InterP16x16LumaFlow()
{
}

void InterP16x16LumaFlow::Frontend()
{
	Predict();
	TransformAndQuantize();
	InverseQuantizeAndTransform();
	Reconstruct();

	CalculateDistortion();
}

void InterP16x16LumaFlow::Backend()
{
	m_predictor->UpdateMotionInfo();
}

uint32_t InterP16x16LumaFlow::OutputMotionInfo(std::shared_ptr<BytesData> bytes_data)
{
	auto start_bit_count = bytes_data->GetBitsCount();
	
	auto mvd = m_predictor->GetMVD();

	CodingUtil::SE_V(mvd.x, bytes_data);
	CodingUtil::SE_V(mvd.y, bytes_data);

	auto finish_bit_count = bytes_data->GetBitsCount();
	return finish_bit_count - start_bit_count;
}

void InterP16x16LumaFlow::Predict()
{
	m_predictor = std::make_unique<InterP16x16LumaPredictor>(m_mb, m_encoder_context);
	m_predictor->Decide();
	m_predictor->FillDiffData(m_diff_datas);
}

void InterP16x16LumaFlow::Reconstruct()
{
	auto predicted_data = m_predictor->GetPredictedData();
	for (uint32_t y = 0; y < 16; ++y)
	{
		for (uint32_t x = 0; x < 16; ++x)
		{
			auto predicted_val = predicted_data.GetElement(x, y);
			auto residual_val = m_cbp == 0 ? 0 : m_diff_data.GetElement(x, y);
			auto val = ReconstructUtil::Reconstruct(predicted_val, residual_val);
			m_reconstructed_data.SetElement(x, y, val);
		}
	}
}

__codec_end