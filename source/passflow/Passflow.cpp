#include "passflow/Passflow.h"

namespace ti::passflow {

unsigned int Passflow::AddPassToFlow(BasePass* pass)
{
    pipelines.emplace_back(std::make_pair(pass, false));
    return static_cast<unsigned int>(pipelines.size() - 1);
}

BasePass* Passflow::GetPassFromFlow(unsigned int index)
{
    if (index < pipelines.size()) {
        return pipelines[index].first;
    }
    return nullptr;
}

bool Passflow::EnablePass(unsigned int index, bool enable)
{
    if (index < pipelines.size()) {
        pipelines[index].second = enable;
        return true;
    }
    return false;
}

bool Passflow::IsEnablePass(unsigned int index)
{
    if (index < pipelines.size()) {
        return pipelines[index].second;
    }
    return false;
}

void Passflow::ExecuteWorkflow()
{
    for (auto& each : pipelines) {
        if (each.second) {
            each.first->BeginPass();
        }
    }

    for (auto& each : pipelines) {
        if (each.second) {
            each.first->ExecutePass();
        }
    }

    for (auto& each : pipelines) {
        if (each.second) {
            each.first->EndPass();
        }
    }
}

}
