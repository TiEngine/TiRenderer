#pragma once

#ifdef TI_OPT_SAFE_DOWN_CAST
#define down_cast dynamic_cast
#else
#define down_cast static_cast
#endif
