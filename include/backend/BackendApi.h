#pragma once

#ifdef _MSC_VER
#ifdef BackendModule
#define BackendApi __declspec(dllexport)
#else
#define BackendApi __declspec(dllimport)
#endif
#else
#define BackendApi __attribute__((visibility("default")))
#endif
