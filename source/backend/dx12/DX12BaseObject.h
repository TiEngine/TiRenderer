#pragma once

#include "DX12Common.h"

namespace ti::backend {
class DX12ObjectCounter : public common::GlobalSingleton<DX12ObjectCounter> {
public:
    using Object = uint32_t;

    Object GetObjectCount() const
    {
        return counter;
    }

    Object Create()
    {
        counter++;
        return (generator++);
    }

    void Destroy()
    {
        counter--;
    }

private:
    #ifdef TI_OPT_MULTI_THREADS
    using ObjectType = std::atomic<Object>;
    #else
    using ObjectType = Object;
    #endif

    ObjectType counter;
    ObjectType generator;
};

class DX12BaseObject {
protected:
    DX12BaseObject()
    {
        id = DX12ObjectCounter::GetReference().Create();
        #if defined(DEBUG) || defined(_DEBUG)
        TI_LOG_D(TAG, "Construct DX12 object `%s` instance `%d`: %p.",
            DX12ObjectUniqueName().c_str(), id, this);
        #endif
    }

    virtual ~DX12BaseObject()
    {
        DX12ObjectCounter::GetReference().Destroy();
        #if defined(DEBUG) || defined(_DEBUG)
        TI_LOG_D(TAG, "Deconstruct DX12 object `%s` instance `%d`: %p.",
            DX12ObjectUniqueName().c_str(), id, this);
        #endif
    }

    virtual std::string DX12ObjectUniqueName() const = 0;

private:
    DX12ObjectCounter::Object id = 0;
};

template <typename Object>
class DX12Object : public DX12BaseObject {
private:
    std::string DX12ObjectUniqueName() const override
    {
        return typeid(Object).name();
    }
};
}
