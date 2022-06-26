#pragma once

#include <memory>        // shared_ptr
#include <unordered_map> // unordered_map
#include "BasePass.h"

namespace ti::passflow {

class DrawItem; // shared_ptr should include!
class FrameResource;

class RasterizePass : public BasePass {
public:
    virtual void AddDrawItem(std::shared_ptr<DrawItem> item);

protected:
    //void PreparePass() override;
    //void BeginPass(backend::CommandRecorder& recorder) override;
    //void ExecutePass(backend::CommandRecorder& recorder) override;
    //void EndPass(backend::CommandRecorder& recorder) override;

    RasterizePass& DeclareInput();
    RasterizePass& DeclareOutput();
    RasterizePass& DeclareBuffer();
    RasterizePass& DeclareTexture();
    void BuildPipeline();

private:
    //std::vector<DrawFrameResource> frameResources;
    std::vector<std::shared_ptr<DrawItem>> drawItems;
    std::unordered_map<backend::CommandRecorder*, FrameResource> frameResources;
};

}
