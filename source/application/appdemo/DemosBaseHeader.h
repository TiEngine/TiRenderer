#pragma once

#include "application/Application.h"

class DemoBase {
public:
    virtual void Begin() = 0;
    virtual void Finish() = 0;
    virtual void Update() = 0;

    DemoBase() = default;
    virtual ~DemoBase() = default;

    inline ti::application::Application& GetApp()
    {
        return application;
    }

private:
    ti::application::Application application;
};

class Demo_01_Backend : public DemoBase {
public:
    virtual void Begin() override;
    virtual void Finish() override;
    virtual void Update() override;
};
