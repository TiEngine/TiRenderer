#pragma once

namespace ti::backend {
class PipelineState {
public:
    struct Description {
    };

protected:
    PipelineState() = default;
    virtual ~PipelineState() = default;
};
}
