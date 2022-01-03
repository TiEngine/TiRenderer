#pragma once

#include <cstdint>

namespace ti::common {

class EntityId {
public:
    using EID = uint64_t;

    explicit EntityId(EID id = 0) : id(id) {}

    operator EID() const
    {
        return id;
    }

private:
    EID id;
};

}
