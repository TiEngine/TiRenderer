#pragma once

#include <d3d12.h>
#include "backend/BasicTypes.h"

namespace ti::backend {

DXGI_FORMAT ConvertFormat(Format format);

unsigned int ConvertMSAA(MSAA msaa);

}
