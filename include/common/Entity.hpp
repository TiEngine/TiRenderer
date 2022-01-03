#pragma once

#include "EntityId.hpp"

namespace ti::common {

class Entity : public EntityId {
public:
    Entity() : EntityId(gid++) {}

    EID GetEntityID()
    {
        return (*this);
    }

    bool IsValid()
    {
        return ((*this) != 0);
    }

private:
    static constexpr EID MAX_INTERNAL_ENTITIES = 100000;
    // Generated ID begin from MAX_INTERNAL_ENTITIES
    static inline EID gid = MAX_INTERNAL_ENTITIES;
};

}
