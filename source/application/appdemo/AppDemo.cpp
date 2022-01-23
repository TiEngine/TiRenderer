#include <Windows.h>
#include <crtdbg.h>
#include <tchar.h>

#include <DirectXMath.h>

#include "application/Application.h"
#include "common/Registry.hpp"

META_COMPONENT(Scale) {
};

DATA_COMPONENT(Scale) {
    DirectX::XMFLOAT4 scale;
};

COMPONENT(Scale);


#ifdef TI_OPT_WINAPP_WINDOW
INT WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PTSTR pCmdLine, INT nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
    #if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    TI_LOG_D("AppDemo", "Enable runtime memory check for debug builds.");
    #endif

    ti::common::Registry registry;
    registry.RegistComponentStorage<ScaleComponent>(new ti::common::ComponentBuffer<ScaleComponent>);
    ti::common::Entity ent = registry.CreateEntity();
    registry.AddComponent<ScaleComponent>(ent);
    registry.ForEach<ScaleComponent>([](ti::common::Entity entity, ScaleComponent& component) {
        component.data.scale.x++;
    });
    ScaleComponent* comp = registry.GetComponent<ScaleComponent>(ent);

    return 0;
}
