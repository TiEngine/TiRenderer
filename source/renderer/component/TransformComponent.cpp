#include "renderer/component/TransformComponent.h"
#include "framework/scene/component/RelationComponent.h"

namespace ti::renderer {

Transform Transform::operator*(const Transform& other) const
{
    using namespace math; // For calling: operator+, operator*.
    Transform result;
    math::XMStoreFloat3(&result.position,
        math::XMLoadFloat3(&this->position) + math::XMLoadFloat3(&other.position));
    math::XMStoreFloat4(&result.rotation,
        math::XMQuaternionMultiply( // q2*q1 (left multiply)
            math::XMLoadFloat4(&this->rotation),   // q1
            math::XMLoadFloat4(&other.rotation))); // q2
    math::XMStoreFloat3(&result.scale,
        math::XMLoadFloat3(&this->scale) * math::XMLoadFloat3(&other.scale));
    return result;
}

void TransformHelper::UpdateGlobalTransform()
{
    auto transformComponent = registry.GetComponent<TransformComponent>(entity);
    if (!transformComponent) {
        TI_LOG_RET_W(TAG, "UpdateGlobalTransform failed because current entity does not has the"
            " TransformComponent! Current entity is " + std::to_string(entity) + ".");
    }

    auto relationComponent = registry.GetComponent<framework::RelationComponent>(entity);
    if (!relationComponent) {
        TI_LOG_RET_W(TAG, "UpdateGlobalTransform failed because current entity does not has the"
            " RelationComponent! Current entity is " + std::to_string(entity) + ".");
    }

    if (relationComponent->data.parent.IsInvalid()) {
        transformComponent->data.global = transformComponent->data.local;
    } else {
        auto parentTransformComponent = registry.GetComponent<TransformComponent>(relationComponent->data.parent);
        if (!parentTransformComponent) {
            TI_LOG_W(TAG, "UpdateGlobalTransform warning: parent entity is valid but not has the"
                " TransformComponent! Current entity is " + std::to_string(entity) + ", and"
                " parent entity is" + std::to_string(relationComponent->data.parent) + ".");
            transformComponent->data.global = transformComponent->data.local;
        } else {
            transformComponent->data.global =
                parentTransformComponent->data.global * transformComponent->data.local;
        }
    }

    std::function<void(common::Entity)> UpdateGlobalTransformForEachChild =
    [UpdateGlobalTransformForEachChild, this](common::Entity entity) {
        auto transformComponent = registry.GetComponent<TransformComponent>(entity);
        auto relationComponent = registry.GetComponent<framework::RelationComponent>(entity);
        if (transformComponent && relationComponent) {
            // The parent node must have the TransformerComponent,
            // because we are updating the child node now. :-)
            auto parentTransformComponent =
                registry.GetComponent<TransformComponent>(relationComponent->data.parent);
            transformComponent->data.global =
                parentTransformComponent->data.global * transformComponent->data.local;
            // Recursively updating for all child nodes.
            for (common::Entity child : relationComponent->data.childs) {
                UpdateGlobalTransformForEachChild(child);
            }
        } else {
            TI_LOG_W(TAG, "UpdateGlobalTransform (for childs) warning: entity is valid but not has the"
                " TransformComponent or RelationComponent! Entity is " + std::to_string(entity) + ".");
        }
    };

    for (common::Entity child : relationComponent->data.childs) {
        UpdateGlobalTransformForEachChild(child);
    }
}

}
