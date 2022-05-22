// TString must be included first.
#include "common/TString.hpp"
#include "DX12Common.h"
#include <comdef.h>

namespace ti::backend {

std::string FormatResult(HRESULT hr)
{
    return std::to_string(_com_error(hr).ErrorMessage());
}

}
