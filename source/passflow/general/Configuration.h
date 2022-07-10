#pragma once

#include "common/Singleton.hpp"
#include "backend/BackendContext.h"

namespace ti::passflow {

class Configuration final : public common::Singleton<Configuration> {
public:
    Configuration();
    ~Configuration();

    bool SetBackendType(backend::BackendContext::Backend type);
    backend::BackendContext::Backend GetBackendType() const;

    bool SetMultipleBufferingCount(unsigned int count);
    unsigned int GetMultipleBufferingCount() const;

private:
    backend::BackendContext::Backend backendType;
    unsigned int multipleBufferingCount = 3;
    // xxxGot is used to mark whether program has called the GetXxx interface.
    // Once the GetXxx interface is called, the configuration is considered
    // complete and the SetXxx interface will no longer take effect.
    mutable bool backendTypeGot = false;
    mutable bool multipleBufferingCountGot = false;
};

}
