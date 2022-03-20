#pragma once

namespace ti::backend {
class ImageSampler {
public:
    struct Description {
    };

protected:
    ImageSampler() = default;
    virtual ~ImageSampler() = default;
};
}
