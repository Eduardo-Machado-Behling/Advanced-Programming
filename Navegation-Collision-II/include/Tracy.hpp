#ifdef TRACY_ENABlE
#include "tracy/Tracy.hpp"
#else
#define ZoneScoped
#define ZoneScopedN(x)
#define FrameMark
#endif