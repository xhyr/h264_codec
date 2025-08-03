#include "neighbor_edge_data.h"

__codec_begin

uint8_t NeighborEdgeData::GetElement(int x, int y) const
{
	if (x == -1 && y == -1)
		return m;

	uint8_t val = CommonConstantValues::s_default_color_value;
	if (x == -1)
	{
		switch (y)
		{
		case 0:
			val = i;
			break;
		case 1:
			val = j;
			break;
		case 2:
			val = k;
			break;
		case 3:
			val = l;
			break;
		}
	}
	else
	{
		switch (x) 
		{
		case 0:
			val = a;
			break;
		case 1:
			val = b;
			break;
		case 2:
			val = c;
			break;
		case 3:
			val = d;
			break;
		case 4:
			val = e;
			break;
		case 5:
			val = f;
			break;
		case 6:
			val = g;
			break;
		case 7:
			val = h;
			break;
		}
	}
	return val;
}

__codec_end


