#include "DemosBaseHeader.h"

class DrawPass : public ti::passflow::RasterizePass {
protected:
    void OnPreparePass(ti::backend::Device* device) override;
    void OnBeginPass(ti::backend::CommandRecorder* recorder) override;
    void OnExecutePass(ti::backend::CommandRecorder* recorder) override;
    void OnEndPass(ti::backend::CommandRecorder* recorder) override;
    void OnEnablePass(bool enable) override;
};

void DrawPass::OnPreparePass(ti::backend::Device* device)
{
    InputProperties inputProperties;
    OutputProperties outputproperties;
    ProgramProperties programProperties;
    ShaderResourceProperties resourceProperties;

    InitializePipeline(device);
}

void DrawPass::OnBeginPass(ti::backend::CommandRecorder* recorder)
{
}

void DrawPass::OnExecutePass(ti::backend::CommandRecorder* recorder)
{
}

void DrawPass::OnEndPass(ti::backend::CommandRecorder* recorder)
{
}

void DrawPass::OnEnablePass(bool enable)
{
}

void Demo_02_Passflow::Begin()
{
    passflow = std::make_unique<ti::passflow::Passflow>("PassflowDemo", 3);
}
