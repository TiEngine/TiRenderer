#include "passflow/BasePass.h"

namespace ti::passflow {

BasePass::BasePass(Passflow& passflow) : passflow(passflow)
{
}

backend::InputVertexAttributes::Attribute
BasePass::InputProperties::MakeDefaultPositionOnlyVertexAttribute()
{
    return { VertexFormat::FLOAT32x3, "POSITION", 0, 0 };
}

}
