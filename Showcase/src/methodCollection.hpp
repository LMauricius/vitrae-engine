#include <iostream>

#include "Vitrae/ComponentRoot.hpp"
#include "Vitrae/Pipelines/Compositing/Task.hpp"
#include "Vitrae/Pipelines/Shading/Task.hpp"
#include "Vitrae/Visuals/Compositor.hpp"

using namespace Vitrae;

struct MethodCollection
{
    dynasma::FirmPtr<Method<ShaderTask>> p_vertexMethod;
    dynasma::FirmPtr<Method<ShaderTask>> p_fragmentMethod;
    dynasma::FirmPtr<Method<ComposeTask>> p_composeMethod;

    virtual ~MethodCollection() = default;
};