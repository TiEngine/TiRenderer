#pragma once

#include "Registry.hpp"

namespace ti::common {
class System {
public:
    virtual void OnCreate() {}
    virtual void OnDestroy() {}

    virtual void OnPrepare(Registry& registry) {}
    virtual void OnFinished(Registry& registry) {}

    virtual void OnPrepTick(Registry& registry, float delta) {}
    virtual void OnPostTick(Registry& registry, float delta) {}

    virtual void OnTick(Registry& registry, float delta) = 0;
};
}
