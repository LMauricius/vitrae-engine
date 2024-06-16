#include "Vitrae/Visuals/Compositor.hpp"
#include "Vitrae/ComponentRoot.hpp"

#include <ranges>

namespace Vitrae
{
class Renderer;

Compositor::Compositor(ComponentRoot &root, dynasma::FirmPtr<Method<ComposeTask>> p_method,
                       dynasma::FirmPtr<FrameStore> p_output)
    : m_root(root), m_pipeline(p_method, {})
{
    m_preparedFrameStores[StandardCompositorOutputNames::OUTPUT] = p_output;

    // here we actually setup the pipeline
    setComposeMethod(p_method);
}
std::size_t Compositor::memory_cost() const
{
    /// TODO: implement
    return sizeof(Compositor);
}

void Compositor::setComposeMethod(dynasma::FirmPtr<Method<ComposeTask>> p_method)
{
    m_pipeline = Pipeline<ComposeTask>(
        p_method, {{PropertySpec{.name = StandardCompositorOutputNames::OUTPUT,
                                 .typeInfo = StandardCompositorOutputTypes::OUTPUT_TYPE}}});

    // clear the buffers (except for the final output)
    auto display_frame = m_preparedFrameStores.at(StandardCompositorOutputNames::OUTPUT);
    m_preparedFrameStores.clear();
    m_preparedTextures.clear();
    m_preparedFrameStores[StandardCompositorOutputNames::OUTPUT] = display_frame;

    // fill the buffers
    for (auto &pipeitem : std::ranges::reverse_view{m_pipeline.items}) {
        pipeitem.p_task->prepareRequiredLocalAssets(m_preparedFrameStores, m_preparedTextures);
    }
}

void Compositor::setDefaultShadingMethod(dynasma::FirmPtr<Method<ShaderTask>> p_vertexMethod,
                                         dynasma::FirmPtr<Method<ShaderTask>> p_fragmentMethod)
{
    m_defaultVertexMethod = p_vertexMethod;
    m_defaultFragmentMethod = p_fragmentMethod;
}

void Compositor::setOutput(dynasma::FirmPtr<FrameStore> p_store)
{
    m_preparedFrameStores[StandardCompositorOutputNames::OUTPUT] = p_store;
}

void Compositor::compose() const
{
    ScopedDict localVars(&parameters);

    // set the output frame property
    localVars.set(StandardCompositorOutputNames::OUTPUT,
                  m_preparedFrameStores.at(StandardCompositorOutputNames::OUTPUT));

    // setup the rendering context
    Renderer &rend = m_root.getComponent<Renderer>();
    RenderRunContext context{.properties = localVars,
                             .renderer = rend,
                             .preparedCompositorFrameStores = m_preparedFrameStores,
                             .preparedCompositorTextures = m_preparedTextures};

    // execute the pipeline
    for (auto &pipeitem : m_pipeline.items) {
        pipeitem.p_task->run(context);
    }

    // sync the framebuffers
    std::set<dynasma::FirmPtr<FrameStore>> uniqueFrameStores;
    for (auto [nameId, p_store] : m_preparedFrameStores) {
        uniqueFrameStores.insert(p_store);
    }
    for (auto p_store : uniqueFrameStores) {
        p_store->sync();
    }
}

} // namespace Vitrae