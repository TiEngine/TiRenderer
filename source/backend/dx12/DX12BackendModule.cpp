// DX12Context.h must be included before other self DX12 headers
// (to make sure that the DirectX header files is included first),
// otherwise the macro of `new` will cause a compilation error!
#include "DX12Context.h"
#include "DX12BaseObject.h"

#ifdef _MSC_VER
#define BACKEND_API_EXPORT __declspec(dllexport)
#else
#define BACKEND_API_EXPORT __attribute__((visibility("default")))
#endif

extern "C" {
BACKEND_API_EXPORT ti::backend::BackendContext* CreateBackend()
{
    TI_LOG_I(ti::backend::TAG, "Create DX12 backend.");
    if (ti::backend::DX12ObjectCounter::GetReference().GetObjectCount() > 0) {
        TI_LOG_W(ti::backend::TAG, "Objects count is not zero when initialize!");
    }
    return new ti::backend::DX12Context;
}

BACKEND_API_EXPORT void DestroyBackend(ti::backend::BackendContext* context)
{
    delete down_cast<ti::backend::DX12Context*>(context);
    if (ti::backend::DX12ObjectCounter::GetReference().GetObjectCount() > 0) {
        TI_LOG_W(ti::backend::TAG, "Objects count is not zero when finalize!");
    }
    TI_LOG_I(ti::backend::TAG, "Destroy DX12 backend.");
}
}
