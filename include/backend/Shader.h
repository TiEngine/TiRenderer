#pragma once

#include <string>
#include "BasicTypes.h"

namespace ti::backend {
class Shader {
public:
    struct Description {
        std::string source;
        enum class SourceType {
            Source,  // source code
            Bytecode // shader bytecode
        } sourceType = SourceType::Source;
        ShaderStage stage = ShaderStage::None;
    };

    virtual std::string DumpBytecode() const = 0;

protected:
    Shader() = default;
    virtual ~Shader() = default;
};
}
