#pragma once

#include "common/Registry.hpp"

namespace ti::framework {

META_COMPONENT(Relation) {
};

DATA_COMPONENT(Relation) {
    common::Entity parent;
    std::vector<common::Entity> childs;
};

COMPONENT(Relation);

class RelationHelper {
LOG_TAG(RelationHelper)

public:
    RelationHelper(common::Registry& registry, common::Entity entity)
        : registry(registry), entity(entity)
    {
    }

    bool Reparent(common::Entity parent = common::Entity());

private:
    common::Registry& registry;
    common::Entity entity;
};

}
