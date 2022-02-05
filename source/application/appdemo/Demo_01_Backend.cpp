#include "DemosBaseHeader.h"

void Demo_01_Backend::Begin()
{
    backend = ti::backend::BackendContext::CreateBackend(ti::backend::BackendContext::Backend::DX12);
    device = backend->CreateDevice();
}

void Demo_01_Backend::Finish()
{
    backend->DestroyDevice(device);
    ti::backend::BackendContext::DestroyBackend(ti::backend::BackendContext::Backend::DX12);
}

void Demo_01_Backend::Update()
{
}
