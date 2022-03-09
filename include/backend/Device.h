#pragma once

#include "Shader.h"
#include "Swapchain.h"
#include "CommandRecorder.h"
#include "InputVertexAttributes.h"
#include "InputIndexAttribute.h"

namespace ti::backend {
class Device {
public:
    struct Description {
        std::string device; // TODO: Selected adapter device name, use default adapter currently.
    };

    virtual Shader* CreateShader(Shader::Description description) = 0;
    virtual bool DestroyShader(Shader* shader) = 0;

    virtual Swapchain* CreateSwapchain(Swapchain::Description description) = 0;
    virtual bool DestroySwapchain(Swapchain* swapchain) = 0;

    virtual CommandRecorder* CreateCommandRecorder(CommandRecorder::Description description) = 0;
    virtual bool DestroyCommandRecorder(CommandRecorder* commandRecorder) = 0;

    virtual InputVertex* CreateInputVertex(InputVertex::Description description) = 0;
    virtual bool DestroyInputVertex(InputVertex* inputVertex) = 0;

    virtual InputVertexAttributes* CreateInputVertexAttributes() = 0;
    virtual bool DestroyInputVertexAttributes(InputVertexAttributes* inputVertexAttributes) = 0;

    virtual InputIndex* CreateInputIndex(InputIndex::Description description) = 0;
    virtual bool DestroyInputIndex(InputIndex* inputIndex) = 0;

    virtual InputIndexAttribute* CreateInputIndexAttribute() = 0;
    virtual bool DestroyInputIndexAttribute(InputIndexAttribute* inputIndexAttribute) = 0;

    virtual ResourceBuffer* CreateResourceBuffer(ResourceBuffer::Description description) = 0;
    virtual bool DestroyResourceBuffer(ResourceBuffer* resourceBuffer) = 0;

    virtual ResourceImage* CreateResourceImage(ResourceImage::Description description) = 0;
    virtual bool DestroyResourceImage(ResourceImage* resourceImage) = 0;

    virtual void WaitIdle() = 0;

protected:
    Device() = default;
    virtual ~Device() = default;
};
}
