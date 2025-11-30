#include "inter_pskip_luma_flow.h"

#include "coding_util.h"
#include "bytes_data.h"
#include "inter_pskip_luma_predictor.h"
#include "encoder_context.h"
#include "motion_info_context.h"
#include "reconstruct_util.h"
#include "inter_luma_flow_base.h"

__codec_begin

InterPSkipLumaFlow::InterPSkipLumaFlow(std::shared_ptr<Macroblock> mb, std::shared_ptr<EncoderContext> encoder_context) :
	InterLumaFlowBase(mb, encoder_context)
{
}

InterPSkipLumaFlow::~InterPSkipLumaFlow()
{
}

void InterPSkipLumaFlow::Frontend()
{
	Predict();
	Reconstruct();

	CalculateDistortion();
}

void InterPSkipLumaFlow::Backend()
{
	m_predictor->UpdateMotionInfo();
}

uint32_t InterPSkipLumaFlow::OutputMotionInfo(std::shared_ptr<BytesData> bytes_data) const 
{
	return 0;
}

void InterPSkipLumaFlow::Predict()
{
	m_predictor = std::make_unique<InterPSkipLumaPredictor>(m_mb, m_encoder_context);
	m_predictor->Decide();
	m_predictor->FillDiffData(m_diff_datas);
}

void InterPSkipLumaFlow::Reconstruct()
{
	auto predicted_data = m_predictor->GetPredictedData();
	for (uint32_t y = 0; y < 16; ++y)
	{
		for (uint32_t x = 0; x < 16; ++x)
		{
			auto predicted_val = predicted_data.GetElement(x, y);
			auto val = ReconstructUtil::Reconstruct(predicted_val, 0);
			m_reconstructed_data.SetElement(x, y, val);
		}
	}
}

__codec_end
