#pragma once

#include "Vitrae/Pipelines/Compositing/Task.hpp"
#include "Vitrae/Pipelines/Pipeline.hpp"
#include "Vitrae/Visuals/Scene.hpp"

#include "dynasma/keepers/abstract.hpp"
#include "dynasma/managers/abstract.hpp"
#include "dynasma/pointer.hpp"

namespace Vitrae
{
class FrameStore;
class ComponentRoot;

class Compositor : public dynasma::PolymorphicBase
{
  public:
    Compositor(ComponentRoot &root, dynasma::FirmPtr<Method<ComposeTask>> p_method,
               dynasma::FirmPtr<FrameStore> p_output);
    virtual ~Compositor() = default;

    void setComposeMethod(dynasma::FirmPtr<Method<ComposeTask>> p_method);
    void setDefaultShadingMethod(dynasma::FirmPtr<Method<ShaderTask>> p_vertexMethod,
                                 dynasma::FirmPtr<Method<ShaderTask>> p_fragmentMethod);
    void setOutput(dynasma::FirmPtr<FrameStore> p_store);

    void compose() const;

    ScopedDict parameters;

  protected:
    ComponentRoot &m_root;
    Pipeline<ComposeTask> m_pipeline;
    std::map<StringId, dynasma::FirmPtr<FrameStore>> m_preparedFrameStores;
    std::map<StringId, dynasma::FirmPtr<Texture>> m_preparedTextures;
    dynasma::FirmPtr<Method<ShaderTask>> m_defaultVertexMethod, m_defaultFragmentMethod;
};

} // namespace Vitrae