#pragma once

#include "common/GlobalSingleton.hpp"
#include "backend/BackendContext.h"

namespace ti::passflow {

class Backend final : public common::GlobalSingleton<Backend> {
public:
    Backend();
    ~Backend();

    backend::Device* Device();

private:
    backend::BackendContext::Backend backend;
    backend::BackendContext* context = nullptr;
    backend::Device*         device  = nullptr;
};

}
