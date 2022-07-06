#pragma once

#include "RasterizePass.h"
#include "ComputePass.h"

namespace ti::passflow {

class Passflow {
public:
    Passflow(std::string flowName, unsigned int multipleBuffering);
    virtual ~Passflow();

    inline unsigned int GetMultipleBufferingCount() const
    {
        return multipleBufferingCount;
    }

    template <typename Pass, typename ...Args>
    Pass* CreateOrGetPass(const std::string& name, const Args& ...args)
    {
        static_assert(std::is_base_of<BasePass, Pass>(), "Pass should inherit from BasePass!");
        if (auto it = passes.find(name); it != passes.end()) {
            return CastPass<Pass>(it->second.get());
        }
        return CastPass<Pass>((passes[name] = std::make_unique<Pass>(*this, args...)).get());
    }

    template <typename Pass>
    Pass* CastPass(BasePass* pass) const
    {
        static_assert(std::is_base_of<BasePass, Pass>(), "Pass should inherit from BasePass!");
        return down_cast<Pass*>(pass); // donot use dynamic_pointer_cast
    }

    unsigned int AddPassToFlow(BasePass* pass);
    BasePass* GetPassFromFlow(unsigned int index);
    bool EnablePass(unsigned int index, bool enable);
    bool IsEnablePass(unsigned int index);

    void ExecuteWorkflow();

private:
    std::string passflowName;

    unsigned int multipleBufferingCount = 0;
    unsigned int currentBufferingIndex = 0;

    std::unordered_map<std::string, std::unique_ptr<BasePass>> passes;
    std::vector<std::pair<BasePass*, bool>> passflow;

    std::vector<std::string> commandRecorderNames;

    backend::Device* bkDevice = nullptr; // Not owned!
    std::vector<backend::CommandRecorder*> bkCommands;
};

}
