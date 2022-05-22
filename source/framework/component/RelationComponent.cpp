#include "framework/scene/component/RelationComponent.h"

namespace ti::framework {

bool RelationHelper::Reparent(common::Entity parent)
{
    auto currentRelation = registry.GetComponent<RelationComponent>(entity);
    if (!currentRelation) {
        TI_LOG_RETF_W(TAG, "Reparent failed because current entity does not has the RelationComponent!"
            " Current entity is %d.", entity);
    }
    if (!currentRelation->data.parent.IsInvalid()) {
        auto oldParentRelation = registry.GetComponent<RelationComponent>(currentRelation->data.parent);
        if (oldParentRelation) {
            auto& childs = oldParentRelation->data.childs;
            auto iter = std::find(childs.begin(), childs.end(), entity);
            if (iter != childs.end()) {
                childs.erase(iter); // erase from old parent
            } else {
                TI_LOG_W(TAG, "Old parent entity %d childs does not has current entity. Current entity is %d.",
                    currentRelation->data.parent, entity);
            }
        } else {
            TI_LOG_W(TAG, "Old parent entity %d does not has the RelationComponent! Current entity is %d.",
                currentRelation->data.parent, entity);
        }
    }
    currentRelation->data.parent = parent; // update new parent
    if (!parent.IsInvalid()) {
        auto parentRelation = registry.GetComponent<RelationComponent>(parent);
        if (parentRelation) {
            parentRelation->data.childs.emplace_back(entity);
        } else {
            TI_LOG_W(TAG, "New parent entity %d does not has the RelationComponent!"
                " Current entity is %d.", parent, entity);
        }
    }
    return true;
}

}
