#include <iostream>

#include "image_util.h"
#include "yuv_frame.h"

using namespace codec;

int main(int argc, char* argv[])
{
	auto yuv_frame = ImageUtil::Read2YUVFrame("test.png");
	yuv_frame->Serial("test.yuv");

	return 0;
}