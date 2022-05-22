#pragma once

#include "GlobalSingletonInstance.hpp"

namespace ti::common {

template <typename T>
class GlobalSingleton : public Singleton<GlobalSingletonInstance<T>> {
public:
    static T& GetReference()
    {
        return Singleton<GlobalSingletonInstance<T>>::GetReference().GetReference();
    }

protected:
    GlobalSingleton() = default;
    ~GlobalSingleton() = default;
};

}
