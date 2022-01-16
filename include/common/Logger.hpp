#pragma once

#ifndef TI_SINGLE_THREAD
#include <thread>
#endif
#include "GlobalSingleton.hpp"

#define TI_LOG_D(tag, what) ti::common::Logger::GetReference().Output("D", tag, what)
#define TI_LOG_I(tag, what) ti::common::Logger::GetReference().Output("I", tag, what)
#define TI_LOG_W(tag, what) ti::common::Logger::GetReference().Output("W", tag, what)
#define TI_LOG_E(tag, what) ti::common::Logger::GetReference().Output("E", tag, what)
#define TI_LOG_F(tag, what) ti::common::Logger::GetReference().Output("F", tag, what)

#define TI_LOG_RETN_D(tag, what) do { TI_LOG_D(tag, what); return nullptr; } while(0)
#define TI_LOG_RETN_I(tag, what) do { TI_LOG_I(tag, what); return nullptr; } while(0)
#define TI_LOG_RETN_W(tag, what) do { TI_LOG_W(tag, what); return nullptr; } while(0)
#define TI_LOG_RETN_E(tag, what) do { TI_LOG_E(tag, what); return nullptr; } while(0)
#define TI_LOG_RETN_F(tag, what) do { TI_LOG_F(tag, what); return nullptr; } while(0)

#define TI_LOG_RETF_D(tag, what) do { TI_LOG_D(tag, what); return false; } while(0)
#define TI_LOG_RETF_I(tag, what) do { TI_LOG_I(tag, what); return false; } while(0)
#define TI_LOG_RETF_W(tag, what) do { TI_LOG_W(tag, what); return false; } while(0)
#define TI_LOG_RETF_E(tag, what) do { TI_LOG_E(tag, what); return false; } while(0)
#define TI_LOG_RETF_F(tag, what) do { TI_LOG_F(tag, what); return false; } while(0)

#define TI_LOG_RET_D(tag, what)  do { TI_LOG_D(tag, what); return; } while(0)
#define TI_LOG_RET_I(tag, what)  do { TI_LOG_I(tag, what); return; } while(0)
#define TI_LOG_RET_W(tag, what)  do { TI_LOG_W(tag, what); return; } while(0)
#define TI_LOG_RET_E(tag, what)  do { TI_LOG_E(tag, what); return; } while(0)
#define TI_LOG_RET_F(tag, what)  do { TI_LOG_F(tag, what); return; } while(0)

namespace ti::common {

class Logger final : public GlobalSingleton<Logger> {
public:
    Logger()
    {
        UpdateCallback(&Logger::DefaultCallback);
    }

    void UpdateCallback(std::function<void(const std::string&, const std::string&, const std::string&)> cb)
    {
        callback = cb;
    }

    void Output(const std::string& level, const std::string& tag, const std::string& what)
    {
        callback(level, tag, what);
    }

private:
    static void DefaultCallback(const std::string& level, const std::string& tag, const std::string& what)
    {
        #ifndef TI_SINGLE_THREAD
        size_t id = std::hash<std::thread::id>()(std::this_thread::get_id());
        #endif
        #ifdef _MSC_VER
        printf_s
        #else
        printf
        #endif
        #ifdef TI_SINGLE_THREAD
        ("[%s] %s - %s", level.c_str(), tag.c_str(), what.c_str());
        #else
        ("[%s][%llu] %s - %s", level.c_str(), id, tag.c_str(), what.c_str());
        #endif
    }

    std::function<void(const std::string&, const std::string&, const std::string&)> callback;
};

}
