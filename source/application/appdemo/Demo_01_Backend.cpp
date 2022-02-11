#include "DemosBaseHeader.h"

void Demo_01_Backend::Begin()
{
    backend = ti::backend::BackendContext::CreateBackend(ti::backend::BackendContext::Backend::DX12);
    device = backend->CreateDevice();
}

void Demo_01_Backend::Finish()
{
    if (swapchain) {
        device->DestroySwapchain(swapchain);
    }
    backend->DestroyDevice(device);
    ti::backend::BackendContext::DestroyBackend(ti::backend::BackendContext::Backend::DX12);
}

void Demo_01_Backend::Update()
{
}

void Demo_01_Backend::Resize(HWND window, unsigned int width, unsigned int height)
{
    if (swapchain) {
        device->DestroySwapchain(swapchain);
    }

    ti::backend::Swapchain::Description swapchainDescription;
    swapchainDescription.width = width;
    swapchainDescription.height = height;
    swapchainDescription.window = window;
    swapchain = device->CreateSwapchain(swapchainDescription);
}
