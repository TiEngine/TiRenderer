#pragma once

#include <typeinfo>
#include "Logger.hpp"
#include "TypeCast.hpp"
#include "Component.hpp"

namespace ti::common {

class Registry final {
LOG_TAG(Registry)

public:
    using TypeHash = decltype(typeid(void).hash_code());

    template <typename Component>
    bool RegistComponentStorage(ComponentStorage* storage)
    {
        #if defined (DEBUG) || defined (_DEBUG)
        componentNameHashMap[typeid(Component).name()] = typeid(Component).hash_code();
        #endif
        TI_LOG_I(TAG, "Regist component storage, type name is `" + std::string(typeid(Component).name())
            + "`, type hash is " + std::to_string(typeid(Component).hash_code()) + ".");
        return storages.emplace(typeid(Component).hash_code(), storage).second;
    }

    template <typename Component>
    ComponentStorage* UnregistComponentStorage()
    {
        #if defined (DEBUG) || defined (_DEBUG)
        componentNameHashMap.erase(typeid(Component).name());
        #endif
        TI_LOG_I(TAG, "Unregist component storage, type name is `" + std::string(typeid(Component).name())
            + "`, type hash is " + std::to_string(typeid(Component).hash_code()) + ".");
        auto storage = storages.find(typeid(Component).hash_code());
        if (storage == storages.end()) {
            return nullptr;
        }
        auto output = storage->second;
        storages.erase(storage);
        return output;
    }

    template <typename Component>
    Component* GetComponent(Entity entity)
    {
        if (entity.IsInvalid()) {
            TI_LOG_RETN_W(TAG, "Get component failed, Input entity is invalid!");
        }
        auto storage = storages.find(typeid(Component).hash_code());
        if (storage == storages.end()) {
            TI_LOG_RETN_W(TAG, "The storage for current component is not yet registered!");
        }
        return static_cast<Component*>(storage->second->GetComponent(entity));
    }

    template <typename Component>
    Component* AddComponent(Entity entity)
    {
        if (entity.IsInvalid()) {
            TI_LOG_RETN_W(TAG, "Add component failed, Input entity is invalid!");
        }
        auto storage = storages.find(typeid(Component).hash_code());
        if (storage == storages.end()) {
            TI_LOG_RETN_W(TAG, "The storage for current component is not yet registered!");
        }
        return static_cast<Component*>(storage->second->AddComponent(entity));
    }

    template <typename Component>
    bool RemoveComponent(Entity entity)
    {
        if (entity.IsInvalid()) {
            TI_LOG_RETF_W(TAG, "Remove component failed, input entity is invalid!");
        }
        auto storage = storages.find(typeid(Component).hash_code());
        if (storage == storages.end()) {
            TI_LOG_RETF_W(TAG, "The storage for current component is not yet registered!");
        }
        return storage->second->RemoveComponent(entity);
    }

    Entity CreateEntity()
    {
        return Entity(gid++);
    }

    void DestroyEntity(Entity entity)
    {
        for (auto& storage : storages) {
            storage.second->RemoveComponent(entity);
        }
    }

    template <typename Component>
    bool ForEach(std::function<void(Entity, Component&)> process)
    {
        auto storage = storages.find(typeid(Component).hash_code());
        if (storage == storages.end()) {
            TI_LOG_RETF_W(TAG, "ForEach failed, the storage is not yet registered!");
        }
        if (auto buffer = down_cast<ComponentBuffer<Component>*>(storage->second)) {
            for (auto& each : buffer->components) {
                process(each.first, each.second);
            }
        }
        return true;
    }

private:
    #if defined (DEBUG) || defined (_DEBUG)
    std::unordered_map<std::string, TypeHash> componentNameHashMap;
    #endif

    std::unordered_map<TypeHash, ComponentStorage*> storages;
    Entity::EntityId gid = 10; // The first ten IDs are reserved.
};

}
