#pragma once

#include <memory>        // unique_ptr
#include <unordered_map> // unordered_map
#include "BasePass.h"

namespace ti::passflow {

class Passflow {
public:
    Passflow(std::string flowName, unsigned int multipleBuffering);
    virtual ~Passflow();

    template <typename Pass, typename ...Args>
    Pass* CreateOrGetPass(const std::string& name, const Args& ...args)
    {
        static_assert(std::is_base_of<BasePass, Pass>::value,
            "Pass should inherit from BasePass!");
        if (auto it = passes.find(name)) {
            return CastPass(it->second);
        }
        return CastPass((passes[name] = std::make_unique<Pass>(*this, args...)).get());
    }

    template <typename Pass>
    Pass* CastPass(BasePass* pass)
    {
        return down_cast<Pass*>(pass);
    }

    unsigned int AddPassToFlow(BasePass* pass);
    BasePass* GetPassFromFlow(unsigned int index);
    bool EnablePass(unsigned int index, bool enable);
    bool IsEnablePass(unsigned int index);

    void ExecuteWorkflow();

private:
    std::unordered_map<std::string, std::unique_ptr<BasePass>> passes;
    std::vector<std::pair<BasePass*, bool>> passflow;

    std::string passflowName;
    unsigned int multipleBufferingCount = 0;
    unsigned int currentBufferingIndex = 0;

    std::vector<std::string> commandRecorderNames;

    backend::Device* bkDevice = nullptr;
    std::vector<backend::CommandRecorder*> bkCommands;
};

}
