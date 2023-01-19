#pragma once

#include "Vitrae/Resources/ShaderStep.h"
#include "Vitrae/Resources/ShaderSteps/Source.h"
#include "Vitrae/Resources/GenericTaskSteps/Group.h"

namespace Vitrae
{
    /**
     * A ShaderStep describes a modular reusable part of a Shader
    */
    using GroupShaderStep = GenericGroupStep<ShaderStep, SourceShaderStep>;
    
}