#pragma once

#include <any>
#include <memory>

namespace ti::application {

class Application {
public:
    enum class Platform {
        Windows
    };

    static std::shared_ptr<Application> CreateApplication(
        Platform platform, std::any args = {});
    static void DestroyApplication();

protected:


private:
    static std::shared_ptr<Application> application;
};

}
