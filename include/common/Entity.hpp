#pragma once

#include <cstdint>

namespace ti::common {

class Entity {
public:
    using EntityId = uint32_t;

    explicit Entity(EntityId id = 0) : id(id)
    {
    }

    bool IsInvalid()
    {
        return (id == 0);
    }

    operator EntityId() const
    {
        return id;
    }

private:
    EntityId id;
};

}

namespace std {

template <>
struct hash<ti::common::Entity> {
    ti::common::Entity::EntityId operator()(const ti::common::Entity& entity) const
    {
        return entity;
    }
};

}
