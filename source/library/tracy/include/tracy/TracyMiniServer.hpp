#ifndef __TRACYMINISERVER_HPP__
#define __TRACYMINISERVER_HPP__

#ifdef TRACY_ENABLE
#include "../common/TracyApi.h"
namespace tracy
{
TRACY_API bool StartMiniServer(const char* file);
TRACY_API void StopMiniServer();
}
#endif
#endif
