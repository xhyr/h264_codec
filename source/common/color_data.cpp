#include "color_data.h"

__codec_begin

ColorData::ColorData():valid(false)
{
}

ColorData::ColorData(int _width, int _height, int _channel_num, uint8_t* _data):
	valid(true), width(_width), height(_height), channel_num(_channel_num), data(_data, _data + width * height * channel_num)
{
}

__codec_end
