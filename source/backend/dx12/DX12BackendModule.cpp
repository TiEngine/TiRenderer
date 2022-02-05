#include "common/TypeCast.hpp"
// DX12Context.h must be included before DX12Common.h (to make sure that
// the DirectX header files is included first), otherwise the macro of
// new will cause a compilation error!
#include "DX12Context.h"
#include "DX12Common.h"

#ifdef _MSC_VER
#define BACKEND_API_EXPORT __declspec(dllexport)
#else
#define BACKEND_API_EXPORT __attribute__((visibility("default")))
#endif

extern "C" {

BACKEND_API_EXPORT ti::backend::BackendContext* CreateBackend()
{
    TI_LOG_I(ti::backend::TAG, "Create dx12 backend.");
    return new ti::backend::DX12Context;
}

BACKEND_API_EXPORT void DestroyBackend(ti::backend::BackendContext* context)
{
    delete down_cast<ti::backend::DX12Context*>(context);
    TI_LOG_I(ti::backend::TAG, "Destroy dx12 backend.");
}

}
