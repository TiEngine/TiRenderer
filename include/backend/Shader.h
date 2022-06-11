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
        } sourceType;

        Description(
            ShaderStage stage,
            std::string source,
            SourceType sourceType = SourceType::Source)
            : stage(stage)
            , source(source)
            , sourceType(sourceType)
        {}
    };

    struct Reflection {
        unsigned int descriptorsCount = 0;
    };

    virtual std::string DumpBytecode() const = 0;

    virtual Reflection Reflect() const = 0;

protected:
    Shader() = default;
    virtual ~Shader() = default;
};

}
