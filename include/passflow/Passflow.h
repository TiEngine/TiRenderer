#pragma once

#include "RasterizePass.h"
#include "ComputePass.h"

namespace ti::passflow {

class Passflow {
public:
    explicit Passflow(std::string name);
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
        // Do not use std::dynamic_pointer_cast because it is highly dependent on the compiler
        // implementation, and different compiler implementations may have different behavior
        // when use it across dynamic libraries, and it will cause cast work incorrectly.
        return down_cast<Pass*>(pass);
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
