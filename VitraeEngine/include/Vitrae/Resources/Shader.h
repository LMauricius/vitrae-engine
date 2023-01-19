#pragma once

#include "Vitrae/Util/Types.h"
#include "Vitrae/ResourceManager.h"
#include "Vitrae/Resources/ShaderStep.h"

namespace Vitrae
{

    class Shader
    {
    public:
        virtual ~Shader() = 0;

        /**
         * @brief Set the Output Step object
         * 
         * @param outputName The name of the output operation the step will be used for
         * Could be "vertex", "geometry", "fragShadedColor", "fragDepthComponent", "fragDiffuseComponent", "fragSpecularComponent"...
         * @param step The step that is used for the operation.
         * The step or its substeps should not be modified while assigned to a Shader
         */
        virtual void setOutputStep(const String& outputName, casted_resource_ptr<ShaderStep> step) = 0;
    };
    
}

