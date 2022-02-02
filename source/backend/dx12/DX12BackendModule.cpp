#include "DX12Context.h"
#include "common/Logger.hpp"
#include "common/TypeCast.hpp"

#ifdef _MSC_VER
#define BACKEND_API_EXPORT __declspec(dllexport)
#else
#define BACKEND_API_EXPORT __attribute__((visibility("default")))
#endif

extern "C" {
BACKEND_API_EXPORT ti::backend::BackendContext* CreateBackend()
{
    return new ti::backend::DX12Context;
}

BACKEND_API_EXPORT void DestroyBackend(ti::backend::BackendContext* context)
{
    delete down_cast<ti::backend::DX12Context*>(context);
}
}
