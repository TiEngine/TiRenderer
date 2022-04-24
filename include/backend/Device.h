#pragma once

#include "Shader.h"
#include "Swapchain.h"
#include "CommandRecorder.h"

namespace ti::backend {
class Device {
public:
    struct Description {
        std::string device; // TODO: Selected adapter device name, use default adapter currently.
    };

    virtual Shader* CreateShader(Shader::Description description) = 0;
    virtual bool DestroyShader(Shader* instance) = 0;

    virtual Swapchain* CreateSwapchain(Swapchain::Description description) = 0;
    virtual bool DestroySwapchain(Swapchain* instance) = 0;

    virtual CommandRecorder* CreateCommandRecorder(CommandRecorder::Description description) = 0;
    virtual bool DestroyCommandRecorder(CommandRecorder* instance) = 0;

    virtual InputVertex* CreateInputVertex(InputVertex::Description description) = 0;
    virtual bool DestroyInputVertex(InputVertex* instance) = 0;

    virtual InputVertexAttributes* CreateInputVertexAttributes() = 0;
    virtual bool DestroyInputVertexAttributes(InputVertexAttributes* instance) = 0;

    virtual InputIndex* CreateInputIndex(InputIndex::Description description) = 0;
    virtual bool DestroyInputIndex(InputIndex* instance) = 0;

    virtual InputIndexAttribute* CreateInputIndexAttribute() = 0;
    virtual bool DestroyInputIndexAttribute(InputIndexAttribute* instance) = 0;

    virtual ResourceBuffer* CreateResourceBuffer(ResourceBuffer::Description description) = 0;
    virtual bool DestroyResourceBuffer(ResourceBuffer* instance) = 0;

    //virtual ResourceBufferEx* CreateResourceBuffer(ResourceBufferEx::Description description) = 0;
    //virtual bool DestroyResourceBuffer(ResourceBufferEx* instance) = 0;

    virtual ResourceImage* CreateResourceImage(ResourceImage::Description description) = 0;
    virtual bool DestroyResourceImage(ResourceImage* instance) = 0;

    //virtual ImageSampler* CreateImageSampler(ImageSampler::Description description) = 0;
    //virtual bool DestroyImageSampler(ImageSampler* instance) = 0;

    virtual DescriptorHeap* CreateDescriptorHeap(DescriptorHeap::Description description) = 0;
    virtual bool DestroyDescriptorHeap(DescriptorHeap* instance) = 0;

    virtual PipelineLayout* CreatePipelineLayout() = 0;
    virtual bool DestroyPipelineLayout(PipelineLayout* instance) = 0;

    virtual PipelineState* CreatePipelineState() = 0;
    virtual bool DestroyPipelineState(PipelineState* instance) = 0;

    virtual void WaitIdle() = 0;

protected:
    Device() = default;
    virtual ~Device() = default;
};
}
