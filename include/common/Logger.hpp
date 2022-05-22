#pragma once

#ifdef TI_OPT_MULTI_THREADS
#include <atomic>
#include <thread>
#endif
#include "GlobalSingleton.hpp"

// Logger.hpp is the header that almost all files include, so
// include the header file for memory leak check in this file.
#include "MemoryLeakChecker.hpp"

#define TI_LOG_D(tag, format, ...) ti::common::Logger::GetReference().Output("D", tag, format, __VA_ARGS__)
#define TI_LOG_I(tag, format, ...) ti::common::Logger::GetReference().Output("I", tag, format, __VA_ARGS__)
#define TI_LOG_W(tag, format, ...) ti::common::Logger::GetReference().Output("W", tag, format, __VA_ARGS__)
#define TI_LOG_E(tag, format, ...) ti::common::Logger::GetReference().Output("E", tag, format, __VA_ARGS__)
#define TI_LOG_F(tag, format, ...) ti::common::Logger::GetReference().Output("F", tag, format, __VA_ARGS__)

#define TI_LOG_RETN_D(tag, format, ...) do { TI_LOG_D(tag, format, __VA_ARGS__); return nullptr; } while(0)
#define TI_LOG_RETN_I(tag, format, ...) do { TI_LOG_I(tag, format, __VA_ARGS__); return nullptr; } while(0)
#define TI_LOG_RETN_W(tag, format, ...) do { TI_LOG_W(tag, format, __VA_ARGS__); return nullptr; } while(0)
#define TI_LOG_RETN_E(tag, format, ...) do { TI_LOG_E(tag, format, __VA_ARGS__); return nullptr; } while(0)
#define TI_LOG_RETN_F(tag, format, ...) do { TI_LOG_F(tag, format, __VA_ARGS__); return nullptr; } while(0)

#define TI_LOG_RETF_D(tag, format, ...) do { TI_LOG_D(tag, format, __VA_ARGS__); return false; } while(0)
#define TI_LOG_RETF_I(tag, format, ...) do { TI_LOG_I(tag, format, __VA_ARGS__); return false; } while(0)
#define TI_LOG_RETF_W(tag, format, ...) do { TI_LOG_W(tag, format, __VA_ARGS__); return false; } while(0)
#define TI_LOG_RETF_E(tag, format, ...) do { TI_LOG_E(tag, format, __VA_ARGS__); return false; } while(0)
#define TI_LOG_RETF_F(tag, format, ...) do { TI_LOG_F(tag, format, __VA_ARGS__); return false; } while(0)

#define TI_LOG_RET_D(tag, format, ...)  do { TI_LOG_D(tag, format, __VA_ARGS__); return; } while(0)
#define TI_LOG_RET_I(tag, format, ...)  do { TI_LOG_I(tag, format, __VA_ARGS__); return; } while(0)
#define TI_LOG_RET_W(tag, format, ...)  do { TI_LOG_W(tag, format, __VA_ARGS__); return; } while(0)
#define TI_LOG_RET_E(tag, format, ...)  do { TI_LOG_E(tag, format, __VA_ARGS__); return; } while(0)
#define TI_LOG_RET_F(tag, format, ...)  do { TI_LOG_F(tag, format, __VA_ARGS__); return; } while(0)

#define LOG_TAG(name) static constexpr char* TAG = #name;

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

    void Output(const char* level, const char* tag, const char* format, ...)
    {
        constexpr int BufferSize = 1024;
        char buffer[BufferSize]{};

        va_list args{};
        va_start(args, format);
        vsnprintf(buffer, BufferSize, format, args);
        va_end(args);

        std::string what(buffer);
        callback(level, tag, what);
    }

private:
    static void DefaultCallback(const std::string& level, const std::string& tag, const std::string& what)
    {
        #ifdef TI_OPT_MULTI_THREADS
        size_t id = std::hash<std::thread::id>()(std::this_thread::get_id());
        #endif
        #ifdef _MSC_VER
        printf_s
        #else
        printf
        #endif
        #ifndef TI_OPT_MULTI_THREADS
        ("[%s]:[%s] %s\n", level.c_str(), tag.c_str(), what.c_str());
        #else
        ("[%llu][%s]:[%s] %s\n", id, level.c_str(), tag.c_str(), what.c_str());
        #endif
    }

    std::function<void(const std::string&, const std::string&, const std::string&)> callback;
};

}
