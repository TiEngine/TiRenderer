#pragma once

#include <memory>
#include <unordered_map>
#include "common/TypeCast.hpp"
#include "BasePass.h"

namespace ti::passflow {

class Passflow {
public:
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
    static constexpr unsigned int FrameCount = 3;

    std::unordered_map<std::string, std::unique_ptr<BasePass>> passes;
    std::vector<std::pair<BasePass*, bool>> passflow;
};

}
