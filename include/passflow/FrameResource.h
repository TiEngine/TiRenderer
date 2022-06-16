#pragma once

#include "backend/BackendContext.h"

namespace ti::passflow {

class BaseConstantBuffer {
};

template <typename T>
class ConstantBuffer : public BaseConstantBuffer {
};

class BaseStructuredBuffer {
};

template <typename T>
class StructuredBuffer : public BaseStructuredBuffer {
};

class BaseReadWriteBuffer {
};

template <typename T>
class ReadWriteBuffer : public BaseReadWriteBuffer {
};

class BaseTexture {
};

class Texture2D : public BaseTexture {
};

class ReadWriteTexture2D : public BaseTexture {
};

class ColorOutput : public BaseTexture {
};

class DepthStencilOutput : public BaseTexture {
};

class DisplayPresentOutput : public BaseTexture {
};

class DrawMesh {
};

class DrawMaterial {
};

}
