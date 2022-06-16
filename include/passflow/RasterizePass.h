#pragma once

#include <vector>
#include "BasePass.h"
#include "DrawFrameResource.h"

namespace ti::framework {

class RasterizePass : public BasePass {
public:

private:
    std::vector<DrawFrameResource> frameResources;
};

}
