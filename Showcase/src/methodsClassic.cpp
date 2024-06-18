#include "methodsClassic.hpp"

#include "Vitrae/Pipelines/Compositing/ClearRender.hpp"
#include "Vitrae/Pipelines/Compositing/FrameToTexture.hpp"
#include "Vitrae/Pipelines/Compositing/Function.hpp"
#include "Vitrae/Pipelines/Compositing/SceneRender.hpp"
#include "Vitrae/Pipelines/Shading/Constant.hpp"
#include "Vitrae/Pipelines/Shading/Function.hpp"

#include "dynasma/standalone.hpp"

MethodsClassic::MethodsClassic(ComponentRoot &root) : MethodCollection(root)
{
    /*
    VERTEX SHADING
    */

    auto p_worldPosition =
        root.getComponent<ShaderFunctionKeeper>().new_asset({ShaderFunction::StringParams{
            .inputSpecs = {PropertySpec{.name = StandardShaderPropertyNames::INPUT_MODEL,
                                        .typeInfo = StandardShaderPropertyTypes::INPUT_MODEL},
                           PropertySpec{.name = StandardVertexBufferNames::POSITION,
                                        .typeInfo = Variant::getTypeInfo<glm::vec3>()}},
            .outputSpecs = {PropertySpec{.name = "position_world",
                                         .typeInfo = Variant::getTypeInfo<glm::vec4>()}},
            .snippet = R"(
                    void vertexWorldPosition(
                        in mat4 mat_model, in vec3 position_mesh,
                        out vec4 position_world
                    ) {
                        position_world = mat_model * vec4(position_mesh, 1.0);
                    }
                )",
            .functionName = "vertexWorldPosition"}});

    auto p_viewPosition =
        root.getComponent<ShaderFunctionKeeper>().new_asset({ShaderFunction::StringParams{
            .inputSpecs =
                {
                    PropertySpec{.name = StandardShaderPropertyNames::INPUT_VIEW,
                                 .typeInfo = StandardShaderPropertyTypes::INPUT_VIEW},
                    PropertySpec{.name = StandardShaderPropertyNames::INPUT_PROJECTION,
                                 .typeInfo = StandardShaderPropertyTypes::INPUT_PROJECTION},
                    PropertySpec{.name = "position_world",
                                 .typeInfo = Variant::getTypeInfo<glm::vec4>()},
                },
            .outputSpecs = {PropertySpec{.name = StandardShaderPropertyNames::VERTEX_OUTPUT,
                                         .typeInfo = StandardShaderPropertyTypes::VERTEX_OUTPUT}},
            .snippet = R"(
                    void vertexViewPosition(
                        in mat4 mat_view, in mat4 mat_proj, in vec4 position_world,
                        out vec4 position_view
                    ) {
                        position_view = mat_proj * mat_view * position_world;
                    }
                )",
            .functionName = "vertexViewPosition"}});

    p_vertexMethod = dynasma::makeStandalone<Method<ShaderTask>>(
        Method<ShaderTask>::MethodParams{.tasks = {p_worldPosition, p_viewPosition}});

    /*
    FRAGMENT SHADING
    */

    auto p_phongCombine =
        root.getComponent<ShaderFunctionKeeper>().new_asset({ShaderFunction::StringParams{
            .inputSpecs =
                {
                    PropertySpec{.name = StandardMaterialTextureNames::DIFFUSE,
                                 .typeInfo = Variant::getTypeInfo<dynasma::FirmPtr<Texture>>()},
                    PropertySpec{.name = StandardVertexBufferNames::TEXTURE_COORD,
                                 .typeInfo = Variant::getTypeInfo<glm::vec2>()},
                },
            .outputSpecs =
                {
                    PropertySpec{.name = StandardShaderPropertyNames::FRAGMENT_OUTPUT,
                                 .typeInfo = StandardShaderPropertyTypes::FRAGMENT_OUTPUT},
                },
            .snippet = R"(
                    void phongCombine(in sampler2D tex_diffuse, in vec2 tex_coord, out vec4 shade) {
                        shade = texture2D(tex_diffuse, tex_coord);
                    }
                )",
            .functionName = "phongCombine"}});

    p_fragmentMethod = dynasma::makeStandalone<Method<ShaderTask>>(
        Method<ShaderTask>::MethodParams{.tasks = {p_phongCombine}});

    /*
    COMPOSING
    */

    // camera matrices extractor
    auto p_extractCameraProperties =
        dynasma::makeStandalone<ComposeFunction>(ComposeFunction::SetupParams{
            .inputSpecs = {{PropertySpec{
                .name = "scene", .typeInfo = Variant::getTypeInfo<dynasma::FirmPtr<Scene>>()}}},
            .outputSpecs = {{
                PropertySpec{.name = "camera_view", .typeInfo = Variant::getTypeInfo<glm::mat4>()},
                PropertySpec{.name = "camera_persp", .typeInfo = Variant::getTypeInfo<glm::mat4>()},
            }},
            .p_function = [](const RenderRunContext &context) {
                auto p_scene = context.properties.get("scene").get<dynasma::FirmPtr<Scene>>();
                auto p_windowFrame = context.properties.get(StandardCompositorOutputNames::OUTPUT)
                                         .get<dynasma::FirmPtr<FrameStore>>();
                context.properties.set("camera_view", p_scene->camera.getViewMatrix());
                context.properties.set("camera_persp",
                                       p_scene->camera.getPerspectiveMatrix(
                                           p_windowFrame->getSize().x, p_windowFrame->getSize().y));
            }});

    auto p_clear = root.getComponent<ComposeClearRenderKeeper>().new_asset(
        {ComposeClearRender::SetupParams{.root = root,
                                         .backgroundColor = glm::vec4(0.0f, 0.5f, 0.8f, 1.0f),
                                         .displayOutputPropertyName = "display_cleared"}});

    auto p_normalRender =
        root.getComponent<ComposeSceneRenderKeeper>().new_asset({ComposeSceneRender::SetupParams{
            .root = root,
            .viewInputPropertyName = "camera_view",
            .perspectiveInputPropertyName = "camera_persp",
            .displayInputPropertyName = "display_cleared",
            .displayOutputPropertyName = StandardCompositorOutputNames::OUTPUT}});

    p_composeMethod =
        dynasma::makeStandalone<Method<ComposeTask>>(Method<ComposeTask>::MethodParams{
            .tasks = {p_extractCameraProperties, p_clear, p_normalRender}});
}
