#pragma once

#include "common/Component.hpp"

namespace ti::framework {

META_COMPONENT(Relation) {
};

DATA_COMPONENT(Relation) {
    common::Entity parent;
    std::vector<common::Entity> childs;
};

EXTRA_COMPONENT(Relation, false);

}
