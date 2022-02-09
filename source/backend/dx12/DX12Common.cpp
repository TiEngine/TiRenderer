#include "DX12Common.h"

namespace ti::backend {

std::string ti::backend::FormatResult(HRESULT hr)
{
    LPVOID message = NULL; // message: Output message string.
    // FORMAT_MESSAGE_ALLOCATE_BUFFER: Allocate buffer for message.
    // FORMAT_MESSAGE_FROM_SYSTEM/HMODULES: Search from system and current process module.
    // MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT): Use user default language for formating.
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE,
        NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message, 0, NULL);
    if (message) {
        std::string output(static_cast<const char*>(message));
        LocalFree(message);
        return output;
    }
    return "Cannot format, see debug layer message for detail.";
}

}
