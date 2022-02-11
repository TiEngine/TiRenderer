#include "backend/BackendContext.h"
#include "common/DllWrapper.hpp"

namespace {
static std::unordered_map<ti::backend::BackendContext::Backend,
    std::pair<ti::common::DllWrapper, ti::backend::BackendContext*>> storage;
}

namespace ti::backend {
BackendContext* BackendContext::CreateBackend(Backend type)
{
    static const std::unordered_map<Backend, std::string> libraries = {
        { Backend::DX12,   "backend_dx12"   },
        { Backend::Vulkan, "backend_vulkan" }
    };

    if (storage.find(type) != storage.end()) {
        return nullptr;
    }

    auto library = libraries.find(type);
    if (library == libraries.end()) {
        return nullptr;
    }

    auto& instance = storage[type]; // backend instance(library wrapper and context pointer)

    instance.first.Load(library->second);
    instance.second = instance.first.ExecuteFunction<BackendContext*()>("CreateBackend");
    return instance.second;
}

void BackendContext::DestroyBackend(Backend type)
{
    auto backend = storage.find(type);
    if (backend == storage.end()) {
        return;
    }

    auto& instance = backend->second;

    instance.first.ExecuteFunction<void(BackendContext*)>("DestroyBackend", instance.second);
    instance.first.UnLoad();

    storage.erase(backend);
}
}
