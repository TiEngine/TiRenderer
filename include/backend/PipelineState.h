#pragma once

namespace ti::backend {
class PipelineState {
public:
    struct Description {
        Description() {}
    };

protected:
    PipelineState() = default;
    virtual ~PipelineState() = default;
};
}
