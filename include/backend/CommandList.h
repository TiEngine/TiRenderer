#pragma once

namespace ti::backend {
class CommandList {
public:
    struct Description {
    };

protected:
    CommandList() = default;
    virtual ~CommandList() = default;
};
}
