// Included exactly once.

#define UNICODE
#include <windows.h>

#ifdef __CYGWIN__
#	define _WIN32
#endif

#ifdef CreateEvent
#	undef CreateEvent
#endif

namespace avo::win {



} // namespace avo::win
