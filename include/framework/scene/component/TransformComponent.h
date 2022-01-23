#pragma once

#include "math/Math.hpp"
#include "common/Component.hpp"

namespace ti::framework {

META_COMPONENT(Transform) {
};

DATA_COMPONENT(Transform) {
    math::XMFLOAT4 position;
    math::XMFLOAT4 rotation;
    math::XMFLOAT4 scale;
};

COMPONENT(Transform);

}
