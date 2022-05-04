#pragma once

#include "DescriptorGroup.h"

namespace ti::backend {
class PipelineLayout {
public:
    struct Description {
        std::string cache; // Unsupported yet
    };

    virtual void AddGroup(DescriptorGroup& group) = 0;
    virtual void BuildLayout() = 0;

protected:
    PipelineLayout() = default;
    virtual ~PipelineLayout() = default;
};
}
