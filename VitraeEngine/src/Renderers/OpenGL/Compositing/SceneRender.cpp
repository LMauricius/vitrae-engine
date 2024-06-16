#include "Vitrae/Renderers/OpenGL/Compositing/SceneRender.hpp"
#include "Vitrae/Assets/FrameStore.hpp"
#include "Vitrae/ComponentRoot.hpp"
#include "Vitrae/Renderers/OpenGL.hpp"
#include "Vitrae/Renderers/OpenGL/FrameStore.hpp"
#include "Vitrae/Renderers/OpenGL/Mesh.hpp"
#include "Vitrae/Renderers/OpenGL/ShaderCompilation.hpp"
#include "Vitrae/Renderers/OpenGL/Texture.hpp"
#include "Vitrae/Util/Property.hpp"
#include "Vitrae/Visuals/Scene.hpp"

namespace Vitrae
{

OpenGLComposeSceneRender::OpenGLComposeSceneRender(const SetupParams &params)
    : ComposeSceneRender(
          std::span<const PropertySpec>{
              {PropertySpec{.name = "scene",
                            .typeInfo = Variant::getTypeInfo<dynasma::FirmPtr<Scene>>()},
               PropertySpec{.name = params.viewInputPropertyName,
                            .typeInfo = Variant::getTypeInfo<glm::mat4>()},
               PropertySpec{.name = params.perspectiveInputPropertyName,
                            .typeInfo = Variant::getTypeInfo<glm::mat4>()}}},
          std::span<const PropertySpec>{
              {PropertySpec{.name = params.displayOutputPropertyName,
                            .typeInfo = Variant::getTypeInfo<dynasma::FirmPtr<FrameStore>>()}}}),
      m_root(params.root), m_viewInputNameId(params.viewInputPropertyName),
      m_perspectiveInputNameId(params.perspectiveInputPropertyName),
      m_displayOutputNameId(params.displayOutputPropertyName)
{}

void OpenGLComposeSceneRender::run(RenderRunContext args) const
{
    OpenGLRenderer &rend = static_cast<OpenGLRenderer &>(m_root.getComponent<Renderer>());
    CompiledGLSLShaderCacher &shaderCacher = m_root.getComponent<CompiledGLSLShaderCacher>();

    Scene &scene = *args.properties.get("scene").get<dynasma::FirmPtr<Scene>>();
    glm::mat4 viewMat = args.properties.get(m_viewInputNameId).get<glm::mat4>();
    glm::mat4 perspectiveMat = args.properties.get(m_perspectiveInputNameId).get<glm::mat4>();

    OpenGLFrameStore &frame = static_cast<OpenGLFrameStore &>(
        *args.properties.get(m_displayOutputNameId).get<dynasma::FirmPtr<FrameStore>>());

    // build map of shaders to materials to mesh props
    std::map<std::pair<dynasma::FirmPtr<Method<ShaderTask>>, dynasma::FirmPtr<Method<ShaderTask>>>,
             std::map<dynasma::FirmPtr<const Material>, std::vector<const MeshProp *>>>
        methods2materials2props;

    for (auto &meshProp : scene.meshProps) {
        auto mat = meshProp.p_mesh->getMaterial().getLoaded();

        methods2materials2props[{mat->getVertexMethod(), mat->getFragmentMethod()}]
                               [meshProp.p_mesh->getMaterial()]
                                   .push_back(&meshProp);
    }

    // test run over shaders
    for (auto &[methods, materials2props] : methods2materials2props) {
        auto [vertexMethod, fragmentMethod] = methods;

        // compile shader for this material
        dynasma::FirmPtr<CompiledGLSLShader> p_compiledShader =
            shaderCacher.retrieve_asset({CompiledGLSLShader::SurfaceShaderSpec{
                .vertexMethod = vertexMethod, .fragmentMethod = fragmentMethod, .root = m_root}});
    }

    frame.startRender({0.0f, 0.0f}, {1.0f, 1.0f});

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render the scene
    // iterate over shaders
    for (auto &[methods, materials2props] : methods2materials2props) {
        auto [vertexMethod, fragmentMethod] = methods;

        // compile shader for this material
        dynasma::FirmPtr<CompiledGLSLShader> p_compiledShader =
            shaderCacher.retrieve_asset({CompiledGLSLShader::SurfaceShaderSpec{
                .vertexMethod = vertexMethod, .fragmentMethod = fragmentMethod, .root = m_root}});

        glUseProgram(p_compiledShader->programGLName);

        // set the uniforms
        int freeBindingIndex = 0;
        GLint glModelMatrixUniformId;
        for (auto [propertyNameId, uniSpec] : p_compiledShader->uniformSpecs) {
            if (propertyNameId == "mat_model") {
                // this is set per model
                glModelMatrixUniformId = uniSpec.glNameId;

            } else if (propertyNameId == "mat_view") {
                glUniformMatrix4fv(uniSpec.glNameId, 1, GL_FALSE, &(viewMat[0][0]));
            } else if (propertyNameId == "mat_proj") {
                glUniformMatrix4fv(uniSpec.glNameId, 1, GL_FALSE, &(perspectiveMat[0][0]));
            } else {
                auto p = args.properties.getPtr(propertyNameId);
                if (p) {
                    rend.getTypeConversion(uniSpec.srcSpec).setUniform(uniSpec.glNameId, *p);
                }
            }
        }

        auto setPropertyToShader = [&](StringId nameId, const Variant &value) {
            if (auto it = p_compiledShader->uniformSpecs.find(nameId);
                it != p_compiledShader->uniformSpecs.end()) {
                rend.getTypeConversion(it->second.srcSpec).setUniform(it->second.glNameId, value);
            } else if (auto it = p_compiledShader->bindingSpecs.find(nameId);
                       it != p_compiledShader->bindingSpecs.end()) {
                rend.getTypeConversion(it->second.srcSpec)
                    .setBinding(it->second.glNameId, freeBindingIndex);
                glUniform1i(it->second.glNameId, freeBindingIndex);
                freeBindingIndex++;
            }
        };

        // iterate over materials
        for (auto [material, props] : materials2props) {

            // get the textures
            std::map<StringId, dynasma::FirmPtr<const OpenGLTexture>> namedTextures;
            for (auto [nameId, p_texture] : material->getTextures()) {
                setPropertyToShader(nameId, p_texture);
            }

            // iterate over meshes
            for (auto p_meshProp : props) {
                OpenGLMesh &mesh = static_cast<OpenGLMesh &>(*p_meshProp->p_mesh);

                glBindVertexArray(mesh.VAO);
                glUniformMatrix4fv(glModelMatrixUniformId, 1, GL_FALSE,
                                   &(p_meshProp->transform.getModelMatrix()[0][0]));
                glDrawElements(GL_TRIANGLES, sizeof(unsigned int) * 3 * mesh.getTriangles().size(),
                               GL_UNSIGNED_INT, 0);
            }
        }

        glUseProgram(0);
    }

    frame.finishRender();
}

void OpenGLComposeSceneRender::prepareRequiredLocalAssets(
    std::map<StringId, dynasma::FirmPtr<FrameStore>> &frameStores,
    std::map<StringId, dynasma::FirmPtr<Texture>> &textures) const
{
    // We just need to check whether the frame store is already prepared
    if (frameStores.find(m_displayOutputNameId) == frameStores.end()) {
        throw std::runtime_error("Frame store not found");
    }
}

} // namespace Vitrae