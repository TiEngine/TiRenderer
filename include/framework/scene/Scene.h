#pragma once

#include "common/Registry.hpp"

namespace ti::framework {

class Scene {
public:
    Scene();
    virtual ~Scene();

    inline common::Registry& GetRegistry();
    inline common::Entity GetSceneRoot();

    common::Entity CreateNode(common::Entity parent = common::Entity());
    void DestroyNode(common::Entity entity);

    bool ReparentNode(common::Entity current, common::Entity parent = common::Entity());

protected:
    static constexpr char* TAG = "Scene";

private:
    common::Registry registry;
    common::Entity root;
};

}
