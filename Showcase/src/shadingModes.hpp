#pragma once

#include <iostream>

#include "Vitrae/ComponentRoot.hpp"
#include "Vitrae/Pipelines/Compositing/Task.hpp"
#include "Vitrae/Pipelines/Shading/Task.hpp"
#include "Vitrae/Visuals/Compositor.hpp"

using namespace Vitrae;

struct ShadingModeSetter
{
    dynasma::FirmPtr<Method<ShaderTask>> p_phong_vertexMethod;
    dynasma::FirmPtr<Method<ShaderTask>> p_phong_fragmentMethod;

    ShadingModeSetter(ComponentRoot &root);
    void setModes(ComponentRoot &root);
};

namespace ShaderModePropertyNames
{
constexpr const char PHONG_SHADE[] = "phong_shade";
}