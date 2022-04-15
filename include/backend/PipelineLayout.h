#pragma once

#include "DescriptorGroup.h"

namespace ti::backend {
class PipelineLayout {
public:
    struct Description {
        unsigned int descriptorGroupCount;
        std::vector<uint8_t> layoutCache;

        Description(
            unsigned int descriptorGroupCount)
            : descriptorGroupCount(descriptorGroupCount)
        {}

        Description(
            std::vector<uint8_t>&& cache)
            : descriptorGroupCount(0u) // Set the descriptorGroupCount to zero if use the cache.
        {
            layoutCache.swap(cache);
        }
    };

    virtual void AddGroup(DescriptorGroup& group) = 0;
    virtual void BuildLayout() = 0;

protected:
    PipelineLayout() = default;
    virtual ~PipelineLayout() = default;
};
}
