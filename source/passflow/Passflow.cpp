#include "passflow/Passflow.h"
#include "general/Backend.h"

namespace ti::passflow {

Passflow::Passflow(std::string flowName, unsigned int multipleBuffering)
{
    passflowName = flowName;
    multipleBufferingCount = multipleBuffering;

    commandRecorderNames.resize(multipleBufferingCount);
    for (unsigned int n = 0; n < multipleBufferingCount; n++) {
        commandRecorderNames[n] = passflowName + "." + std::to_string(n);
    }

    bkDevice = Backend::GetReference().Device();
    bkCommands.resize(multipleBufferingCount);
    for (unsigned int n = 0; n < multipleBufferingCount; n++) {
        bkCommands[n] = bkDevice->CreateCommandRecorder({
            commandRecorderNames[n], CommandType::Graphics });
    }
}

Passflow::~Passflow()
{
    bkDevice->WaitIdle();

    for (const auto& name : commandRecorderNames) {
        bkDevice->ReleaseCommandRecordersMemory(name);
    }

    for (auto recorder : bkCommands) {
        bkDevice->DestroyCommandRecorder(recorder);
    }
}

unsigned int Passflow::AddPassToFlow(BasePass* pass)
{
    passflow.emplace_back(std::make_pair(pass, false)).first->PreparePass();
    return static_cast<unsigned int>(passflow.size() - 1);
}

BasePass* Passflow::GetPassFromFlow(unsigned int index)
{
    if (index < passflow.size()) {
        return passflow[index].first;
    }
    return nullptr;
}

bool Passflow::EnablePass(unsigned int index, bool enable)
{
    if (index < passflow.size()) {
        passflow[index].second = enable;
        return true;
    }
    return false;
}

bool Passflow::IsEnablePass(unsigned int index)
{
    if (index < passflow.size()) {
        return passflow[index].second;
    }
    return false;
}

void Passflow::ExecuteWorkflow()
{
    bkCommands[currentBufferingIndex]->Wait();
    bkCommands[currentBufferingIndex]->BeginRecord();

    for (auto& each : passflow) {
        if (each.second) {
            each.first->BeginPass(*bkCommands[currentBufferingIndex]);
            each.first->ExecutePass(*bkCommands[currentBufferingIndex]);
            each.first->EndPass(*bkCommands[currentBufferingIndex]);
        }
    }

    bkCommands[currentBufferingIndex]->EndRecord();
    bkCommands[currentBufferingIndex]->Submit();

    currentBufferingIndex = (currentBufferingIndex + 1) % multipleBufferingCount;
}

}