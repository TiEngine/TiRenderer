#pragma once

#include "common/Logger.hpp"

#define LogOutIfFailed(level, expression, success)             \
do {                                                           \
    HRESULT hr = (expression);                                 \
    success = SUCCEEDED(hr);                                   \
    if(!success) {                                             \
        ti::common::Logger::GetReference().Output(#level, TAG, \
            "[ERROR: %d: %s |LOCATION: %s: %d. |EXPRESS: %s]", \
            hr, ti::backend::FormatResult(hr).c_str(),         \
            __FILE__, __LINE__, #expression);                  \
    }                                                          \
} while(0)

#define LogIfFailed(level, expression)          \
do {                                            \
    bool success = false;                       \
    LogOutIfFailed(level, expression, success); \
} while(0)

#define LogOutIfFailedD(expression, success) LogOutIfFailed(D, expression, success)
#define LogOutIfFailedI(expression, success) LogOutIfFailed(I, expression, success)
#define LogOutIfFailedW(expression, success) LogOutIfFailed(W, expression, success)
#define LogOutIfFailedE(expression, success) LogOutIfFailed(E, expression, success)
#define LogOutIfFailedF(expression, success) LogOutIfFailed(F, expression, success)

#define LogIfFailedD(expression) LogIfFailed(D, expression)
#define LogIfFailedI(expression) LogIfFailed(I, expression)
#define LogIfFailedW(expression) LogIfFailed(W, expression)
#define LogIfFailedE(expression) LogIfFailed(E, expression)
#define LogIfFailedF(expression) LogIfFailed(F, expression)

#define ReleaseCOM(com) \
do {                    \
    if (com) {          \
        com->Release(); \
        com = NULL;     \
    }                   \
} while(0)

namespace ti::backend {

LOG_TAG(DX12Backend)

// Format HRESULT to message string.
std::string FormatResult(HRESULT hr);

}
