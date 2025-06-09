#include <iostream>

#include "image_util.h"
#include "yuv_frame.h"
#include "log.h"

using namespace codec;

int main(int argc, char* argv[])
{
	LOGINFO("codec begin.");

	auto yuv_frame = ImageUtil::Read2YUVFrame("test.png");
	yuv_frame->Serial("test.yuv");

	LOGINFO("codec end.");

	return 0;
}