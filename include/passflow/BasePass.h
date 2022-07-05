#pragma once

#include <map> // ordered hash map
#include "backend/BackendContext.h"

namespace ti::passflow {

class Passflow;

class BasePass {
public:
    virtual ~BasePass() = default;

    virtual void OnPreparePass(backend::Device* device) = 0;
    virtual void OnBeforePass(unsigned int currentBufferingIndex) = 0;
    virtual void OnExecutePass(backend::CommandRecorder* recorder) = 0;
    virtual void OnAfterPass(unsigned int currentPassInFlowIndex) = 0;
    virtual void OnEnablePass(bool enable) = 0;

protected:
    explicit BasePass(Passflow& passflow);

    struct InputProperties {
        backend::InputIndexAttribute::Attribute indexAttribute;
        std::vector<backend::InputVertexAttributes::Attribute> vertexAttributes;
        static backend::InputVertexAttributes::Attribute MakeDefaultPositionOnlyVertexAttribute();
    };

    struct OutputProperties {
        enum class OutputSlot : int8_t {
            DS = -1,
            C0 = 0,
            C1 = 1,
            C2 = 2,
            C3 = 3,
            C4 = 4,
            C5 = 5,
            C6 = 6,
            C7 = 7
        };
        struct OutputAttribute {
            BasicFormat imagePixelFormat;
            PassAction beginAction;
            PassAction endAction;
        };
        std::map<OutputSlot, OutputAttribute> targets;
    };

    struct ProgramProperties {
        std::map<ShaderStage, std::string> shaders;
    };

    struct ShaderResourceProperties {
        enum class ResourceSpace : uint8_t {
            PerObject = 0,
            PerScene  = 1,
            PerView   = 2,
            PerPass   = 3
        };
        struct ResourceAttribute {
            unsigned int baseBindingPoint;
            unsigned int bindingPointCount;
            ShaderStage resourceVisibility;
            DescriptorType resourceType;
            ResourceState beforeState;
            ResourceState afterState;
        };
        std::map<ResourceSpace, std::vector<ResourceAttribute>> resources;
    };

    Passflow& passflow;
};

}
