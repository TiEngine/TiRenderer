#pragma once

#include "CommandList.h"

namespace ti::backend {
class CommandAllocator {
public:
    struct Description {
        Description() {}
    };

    virtual CommandList* CreateCommandList(CommandList::Description description) = 0;
    virtual bool DestroyCommandList(CommandList* commandList) = 0;

protected:
    CommandAllocator() = default;
    virtual ~CommandAllocator() = default;
};
}
