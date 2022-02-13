#pragma once

#include "backend/BasicTypes.h"
#include "DX12BackendHeaders.h"

namespace ti::backend {

DXGI_FORMAT ConvertFormat(Format format);

unsigned int ConvertMSAA(MSAA msaa);

}
