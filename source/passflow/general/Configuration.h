#pragma once

#include "common/Singleton.hpp"
#include "backend/BackendContext.h"

namespace ti::passflow {

class Configuration final : public common::Singleton<Configuration> {
public:
    Configuration();
    ~Configuration();

    void SetBackendType(backend::BackendContext::Backend type);
    backend::BackendContext::Backend GetBackendType() const;

private:
    backend::BackendContext::Backend backendType;
};

}
