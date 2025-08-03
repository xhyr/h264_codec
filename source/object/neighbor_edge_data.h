#pragma once

#include <cstdint>

#include "common_constant_values.h"

__codec_begin

struct NeighborEdgeData
{
	uint8_t m{ CommonConstantValues::s_default_color_value };
	
	uint8_t a{ CommonConstantValues::s_default_color_value };
	uint8_t b{ CommonConstantValues::s_default_color_value };
	uint8_t c{ CommonConstantValues::s_default_color_value };
	uint8_t d{ CommonConstantValues::s_default_color_value };
	uint8_t e{ CommonConstantValues::s_default_color_value };
	uint8_t f{ CommonConstantValues::s_default_color_value };
	uint8_t g{ CommonConstantValues::s_default_color_value };
	uint8_t h{ CommonConstantValues::s_default_color_value };
	
	uint8_t i{ CommonConstantValues::s_default_color_value };
	uint8_t j{ CommonConstantValues::s_default_color_value };
	uint8_t k{ CommonConstantValues::s_default_color_value };
	uint8_t l{ CommonConstantValues::s_default_color_value };

	uint8_t GetElement(int x, int y) const;
	
};

__codec_end