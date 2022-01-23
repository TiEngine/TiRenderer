#include "framework/scene/Scene.h"
#include "framework/scene/component/RelationComponent.h"

namespace ti::framework {

Scene::Scene()
{
    // Regist the RelationComponentBuffer in Scene's
    // construction, so it must be successful.
    registry.RegistComponentStorage<RelationComponent>(
        new common::ComponentBuffer<RelationComponent>);
    // Build the root object(entity and node).
    root = registry.CreateEntity();
    registry.AddComponent<RelationComponent>(root);
}

Scene::~Scene()
{
    DestroyNode(root);
    registry.DestroyEntity(root);
    auto relationComponentStorage = registry.
        UnregistComponentStorage<RelationComponent>();
    if (relationComponentStorage) {
        delete relationComponentStorage;
        relationComponentStorage = nullptr;
    } else {
        TI_LOG_W(TAG, "RelationComponentBuffer has been unregisted!");
    }
}

inline common::Registry& Scene::GetRegistry()
{
    return registry;
}

inline common::Entity Scene::GetSceneRoot()
{
    return root;
}

common::Entity Scene::CreateNode(common::Entity parent)
{
    common::Entity entity = registry.CreateEntity();
    auto relation = registry.AddComponent<RelationComponent>(entity);
    if (relation) {
        relation->data.parent = parent;
        if (!parent.IsInvalid()) {
            auto parentRelation = registry.GetComponent<RelationComponent>(parent);
            if (parentRelation) {
                parentRelation->data.childs.emplace_back(entity);
            } else {
                TI_LOG_W(TAG, "Parent entity " + std::to_string(parent) + " does not has the"
                    " RelationComponent! Current entity is " + std::to_string(entity) + ".");
            }
        }
    }
    return entity;
}

void Scene::DestroyNode(common::Entity entity)
{
    auto relation = registry.GetComponent<RelationComponent>(entity);
    if (!relation) {
        TI_LOG_RET_W(TAG, "DestroyNode processing: current entity " + std::to_string(entity) +
            " does not has RelationComponent! Destroy occurr error at this entity.");
    }
    for (common::Entity child : relation->data.childs) {
        DestroyNode(child); // recursively destroy all children
    }
    registry.DestroyEntity(entity);
}

bool Scene::ReparentNode(common::Entity current, common::Entity parent)
{
    auto currentRelation = registry.GetComponent<RelationComponent>(current);
    if (!currentRelation) {
        TI_LOG_RETF_W(TAG, "ReparentNode failed because current entity does not has the RelationComponent!"
            " Current entity is " + std::to_string(current) + ".");
    }
    if (!currentRelation->data.parent.IsInvalid()) {
        auto oldParentRelation = registry.GetComponent<RelationComponent>(currentRelation->data.parent);
        if (oldParentRelation) {
            auto& childs = oldParentRelation->data.childs;
            auto iter = std::find(childs.begin(), childs.end(), current);
            if (iter != childs.end()) {
                childs.erase(iter); // erase from old parent
            } else {
                TI_LOG_W(TAG, "Old parent entity " + std::to_string(currentRelation->data.parent) + " childs"
                    " does not has current entity. Current entity is " + std::to_string(current) + ".");
            }
        } else {
            TI_LOG_W(TAG, "Old parent entity " + std::to_string(currentRelation->data.parent) +
                " does not has the RelationComponent! Current entity is " + std::to_string(current) + ".");
        }
    }
    currentRelation->data.parent = parent; // update new parent
    if (!parent.IsInvalid()) {
        auto parentRelation = registry.GetComponent<RelationComponent>(parent);
        if (parentRelation) {
            parentRelation->data.childs.emplace_back(current);
        } else {
            TI_LOG_W(TAG, "New parent entity " + std::to_string(parent) + " does not has the"
                " RelationComponent! Current entity is " + std::to_string(current) + ".");
        }
    }
    return true;
}

}
