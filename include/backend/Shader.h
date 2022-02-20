#pragma once

#include <string>
#include "BasicTypes.h"

namespace ti::backend {
class Shader {
public:
    struct Description {
        ShaderStage stage;
        std::string source;
        enum class SourceType {
            Source,  // source code
            Bytecode // shader bytecode
        } sourceType = SourceType::Source;

        Description(ShaderStage stage, std::string source,
            SourceType sourceType = SourceType::Source)
            : stage(stage), source(source)
            , sourceType(sourceType) {}

        Description() : stage(ShaderStage::Vertex) {}
    };

    virtual std::string DumpBytecode() const = 0;

protected:
    Shader() = default;
    virtual ~Shader() = default;
};
}
