#pragma once

#include "../shadingModes.hpp"
#include "abstract.hpp"

#include "GI/Generation.hpp"
#include "GI/Probe.hpp"

#include "Vitrae/Pipelines/Compositing/ClearRender.hpp"
#include "Vitrae/Pipelines/Compositing/FrameToTexture.hpp"
#include "Vitrae/Pipelines/Compositing/Function.hpp"
#include "Vitrae/Pipelines/Compositing/SceneRender.hpp"
#include "Vitrae/Pipelines/Shading/Constant.hpp"
#include "Vitrae/Pipelines/Shading/Function.hpp"

#include "dynasma/standalone.hpp"

#include <iostream>

using namespace Vitrae;

struct MethodsAmbientGI : MethodCollection
{
    inline MethodsAmbientGI(ComponentRoot &root) : MethodCollection(root)
    {
        /*
        VERTEX SHADING
        */

        p_vertexMethod = dynasma::makeStandalone<Method<ShaderTask>>(
            Method<ShaderTask>::MethodParams{.tasks = {}});

        /*
        FRAGMENT SHADING
        */

        auto p_shadeAmbient =
            root.getComponent<ShaderFunctionKeeper>().new_asset({ShaderFunction::StringParams{
                .inputSpecs =
                    {
                        PropertySpec{.name = "light_color_ambient",
                                     .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                    },
                .outputSpecs =
                    {
                        PropertySpec{.name = "shade_ambient",
                                     .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                    },
                .snippet = R"(
                    void setGlobalLighting(
                        in vec3 light_color_ambient,
                        out vec3 shade_ambient
                    ) {
                        shade_ambient = light_color_ambient;
                    }
                )",
                .functionName = "setGlobalLighting"}});

        p_fragmentMethod =
            dynasma::makeStandalone<Method<ShaderTask>>(Method<ShaderTask>::MethodParams{
                .tasks = {p_shadeAmbient}, .friendlyName = "GlobalIllumination"});

        /*
        COMPOSING
        */

        p_composeMethod = dynasma::makeStandalone<Method<ComposeTask>>(
            Method<ComposeTask>::MethodParams{.tasks = {}, .friendlyName = "GlobalIllumination"});
    }
};