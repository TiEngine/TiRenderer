#pragma once

#include "math/Math.hpp"
#include "common/Registry.hpp"

namespace ti::renderer {

struct Transform {
    math::XMFLOAT3 position{};
    math::XMFLOAT4 rotation{};
    math::XMFLOAT3 scale{};

    Transform operator*(const Transform& other) const;
};

META_COMPONENT(Transform) {
    Transform initial;
};

DATA_COMPONENT(Transform) {
    Transform local;
    Transform global;
};

COMPONENT(Transform);

class TransformHelper {
LOG_TAG(TransformHelper)

public:
    TransformHelper(common::Registry& registry, common::Entity entity)
        : registry(registry), entity(entity)
    {
    }

    void UpdateGlobalTransform();

private:
    common::Registry& registry;
    common::Entity entity;
};

}
