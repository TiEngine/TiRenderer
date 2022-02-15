#include "DemosBaseHeader.h"

void Demo_01_Backend::Begin()
{
    backend = ti::backend::BackendContext::CreateBackend(ti::backend::BackendContext::Backend::DX12);
    device = backend->CreateDevice({});

    commandAllocator = device->CreateCommandAllocator({});
    commandList = commandAllocator->CreateCommandList({});
}

void Demo_01_Backend::Finish()
{
    if (swapchain) {
        device->DestroySwapchain(swapchain);
    }

    commandAllocator->DestroyCommandList(commandList);
    device->DestroyCommandAllocator(commandAllocator);

    backend->DestroyDevice(device);
    ti::backend::BackendContext::DestroyBackend(ti::backend::BackendContext::Backend::DX12);
}

void Demo_01_Backend::Update()
{
}

void Demo_01_Backend::Resize(HWND window, unsigned int width, unsigned int height)
{
    if (swapchain) {
        device->WaitIdle();
        swapchain->Resize(width, height);
    } else {
        ti::backend::Swapchain::Description swapchainDescription;
        swapchainDescription.width = width;
        swapchainDescription.height = height;
        swapchainDescription.window = window;
        swapchain = device->CreateSwapchain(swapchainDescription);
    }
}
