#include "methodsClassic.hpp"
#include "shadingModes.hpp"

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

    auto p_shadowPosition =
        root.getComponent<ShaderFunctionKeeper>().new_asset({ShaderFunction::StringParams{
            .inputSpecs =
                {
                    PropertySpec{.name = "position_world",
                                 .typeInfo = Variant::getTypeInfo<glm::vec4>()},
                    PropertySpec{.name = "mat_shadow_view",
                                 .typeInfo = Variant::getTypeInfo<glm::mat4>()},
                    PropertySpec{.name = "mat_shadow_persp",
                                 .typeInfo = Variant::getTypeInfo<glm::mat4>()},
                },
            .outputSpecs =
                {
                    PropertySpec{.name = "position_shadow",
                                 .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                },
            .snippet = R"(
                    void shadowPosition(
                        in vec4 position_world, in mat4 mat_shadow_view, in mat4 mat_shadow_persp,
                        out vec3 position_shadow
                    ) {
                        vec4 position_shadow_h = mat_shadow_persp * mat_shadow_view * position_world;
                        position_shadow = position_shadow_h.xyz / position_shadow_h.w * 0.5 + 0.5;
                    }
                )",
            .functionName = "shadowPosition"}});

    p_vertexMethod = dynasma::makeStandalone<Method<ShaderTask>>(Method<ShaderTask>::MethodParams{
        .tasks = {p_worldPosition, p_shadowPosition, p_viewPosition}});

    /*
    FRAGMENT SHADING
    */

    auto p_shadowLightFactor =
        root.getComponent<ShaderFunctionKeeper>().new_asset({ShaderFunction::StringParams{
            .inputSpecs =
                {
                    PropertySpec{.name = "tex_shadow",
                                 .typeInfo = Variant::getTypeInfo<dynasma::FirmPtr<Texture>>()},
                    PropertySpec{.name = "position_shadow",
                                 .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                },
            .outputSpecs =
                {
                    PropertySpec{.name = "light_shadow_factor",
                                 .typeInfo = Variant::getTypeInfo<float>()},
                },
            .snippet = R"(
                    float getLightShadowFactorPixel(
                        in sampler2D tex_shadow, in ivec2 coord, float depth
                    ) {
                        if (texelFetch(tex_shadow, coord, 0).r < depth) {
                            return 0.0;
                        }
                        else {
                            return 1.0;
                        }
                    }
                    void lightShadowFactor(
                        in sampler2D tex_shadow, in vec3 position_shadow,
                        out float light_shadow_factor)
                    {
                        vec2 shadowSize = textureSize(tex_shadow, 0);
                        vec2 coord = position_shadow.xy * shadowSize;
                        ivec2 minCoord = ivec2(floor(coord));
                        ivec2 maxCoord = ivec2(ceil(coord));
                        vec2 slider = coord - vec2(minCoord);
                        light_shadow_factor = 0;
                        for (int i = -1; i <= 1; ++i) {
                            for (int j = -1; j <= 1; ++j) {
                                ivec2 offset = ivec2(i, j);
                                light_shadow_factor += (
                                        getLightShadowFactorPixel(tex_shadow, minCoord + offset, position_shadow.z) * (1.0 - slider.x) +
                                        getLightShadowFactorPixel(tex_shadow, ivec2(maxCoord.x, minCoord.y) + offset, position_shadow.z) * slider.x
                                    ) * (1.0 - slider.y) + (
                                        getLightShadowFactorPixel(tex_shadow, ivec2(minCoord.x, maxCoord.y) + offset, position_shadow.z) * (1.0 - slider.x) +
                                        getLightShadowFactorPixel(tex_shadow, maxCoord + offset, position_shadow.z) * slider.x
                                    ) * slider.y;
                            }
                        }
                        light_shadow_factor /= 9.0;
                    }
                )",
            .functionName = "lightShadowFactor"}});

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
                    void setAmbient(
                        in vec3 light_color_ambient,
                        out vec3 shade_ambient
                    ) {
                        shade_ambient = light_color_ambient;
                    }
                )",
            .functionName = "setAmbient"}});

    auto p_shadeDiffuse =
        root.getComponent<ShaderFunctionKeeper>().new_asset({ShaderFunction::StringParams{
            .inputSpecs =
                {
                    PropertySpec{.name = "normal", .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                    PropertySpec{.name = "light_direction",
                                 .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                    PropertySpec{.name = "light_color_primary",
                                 .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                    PropertySpec{.name = "light_shadow_factor",
                                 .typeInfo = Variant::getTypeInfo<float>()},
                },
            .outputSpecs =
                {
                    PropertySpec{.name = "shade_diffuse",
                                 .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                },
            .snippet = R"(
                    void lightDiffuse(
                        in vec3 normal, in vec3 light_direction,
                        in vec3 light_color_primary, in float light_shadow_factor,
                        out vec3 shade_diffuse
                    ) {
                        shade_diffuse = max(0.0, -dot(light_direction, normal)) * light_color_primary * light_shadow_factor;
                    }
                )",
            .functionName = "lightDiffuse"}});

    auto
        p_shadeSpecular =
            root.getComponent<ShaderFunctionKeeper>()
                .new_asset(
                    {ShaderFunction::StringParams{
                        .inputSpecs =
                            {
                                PropertySpec{.name = "camera_position",
                                             .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                                PropertySpec{.name = "position_world",
                                             .typeInfo = Variant::getTypeInfo<glm::vec4>()},
                                PropertySpec{.name = StandardMaterialTextureNames::SPECULAR,
                                             .typeInfo =
                                                 Variant::getTypeInfo<dynasma::FirmPtr<Texture>>()},
                                PropertySpec{.name = StandardVertexBufferNames::TEXTURE_COORD,
                                             .typeInfo = Variant::getTypeInfo<glm::vec2>()},
                                PropertySpec{.name = "normal",
                                             .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                                PropertySpec{.name = "light_direction",
                                             .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                                PropertySpec{.name = "light_color_primary",
                                             .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                                PropertySpec{.name = "light_shadow_factor",
                                             .typeInfo = Variant::getTypeInfo<float>()},
                            },
                        .outputSpecs =
                            {
                                PropertySpec{.name = "shade_specular",
                                             .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                            },
                        .snippet = R"(
                    void lightSpecular(
                        in vec3 camera_position, in vec4 position_world,
                        in sampler2D tex_specular, in vec2 tex_coord,
                        in vec3 normal, in vec3 light_direction,
                        in vec3 light_color_primary, in float light_shadow_factor,
                        out vec3 shade_specular
                    ) {
                        vec4 color_specular = texture2D(tex_specular, tex_coord);
                        vec3 dirToEye = camera_position - position_world.xyz;
                        vec3 reflRay = 2 * dot(-light_direction, normal) * normal + light_direction;
                        shade_specular =
                            pow(max(dot(reflRay, dirToEye), 0.0), 1.0 / color_specular.a) * light_shadow_factor *
                            color_specular.rgb * light_color_primary;
                    }
                )",
                        .functionName = "lightSpecular"}});

    auto p_phongCombine =
        root.getComponent<ShaderFunctionKeeper>().new_asset(
            {ShaderFunction::StringParams{
                .inputSpecs =
                    {
                        PropertySpec{.name = "shade_diffuse",
                                     .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                        PropertySpec{.name = "shade_specular",
                                     .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                        PropertySpec{.name = "shade_ambient",
                                     .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                        PropertySpec{.name = StandardMaterialTextureNames::DIFFUSE,
                                     .typeInfo = Variant::getTypeInfo<dynasma::FirmPtr<Texture>>()},
                        PropertySpec{.name = StandardVertexBufferNames::TEXTURE_COORD,
                                     .typeInfo = Variant::getTypeInfo<glm::vec2>()},
                    },
                .outputSpecs =
                    {
                        PropertySpec{.name = ShaderModePropertyNames::PHONG_SHADE,
                                     .typeInfo = StandardShaderPropertyTypes::FRAGMENT_OUTPUT},
                    },
                .snippet = R"(
                    void phongCombine(
                        in vec3 shade_diffuse, in vec3 shade_specular, in vec3 shade_ambient,
                        in sampler2D tex_diffuse,
                        in vec2 tex_coord,
                        out vec4 phong_shade
                    ) {
                        vec4 color_diffuse = texture2D(tex_diffuse, tex_coord);
                        phong_shade = vec4(
                            color_diffuse.rgb * (shade_diffuse + shade_ambient) +
                            shade_specular, 
                            color_diffuse.a);
                    }
                )",
                .functionName = "phongCombine"}});

    p_fragmentMethod = dynasma::makeStandalone<Method<ShaderTask>>(Method<ShaderTask>::MethodParams{
        .tasks = {p_shadowLightFactor, p_shadeAmbient, p_shadeDiffuse, p_shadeSpecular,
                  p_phongCombine}});

    /*
    COMPOSING
    */

    // shadow matrices extractor
    auto p_extractLightProperties = dynasma::makeStandalone<
        ComposeFunction>(ComposeFunction::SetupParams{
        .inputSpecs = {{PropertySpec{.name = "scene",
                                     .typeInfo = Variant::getTypeInfo<dynasma::FirmPtr<Scene>>()}}},
        .outputSpecs = {{
            PropertySpec{.name = "mat_shadow_view", .typeInfo = Variant::getTypeInfo<glm::mat4>()},
            PropertySpec{.name = "mat_shadow_persp", .typeInfo = Variant::getTypeInfo<glm::mat4>()},
            PropertySpec{.name = "light_direction", .typeInfo = Variant::getTypeInfo<glm::vec3>()},
            PropertySpec{.name = "light_color_primary",
                         .typeInfo = Variant::getTypeInfo<glm::vec3>()},
            PropertySpec{.name = "light_color_ambient",
                         .typeInfo = Variant::getTypeInfo<glm::vec3>()},
        }},
        .p_function = [](const RenderRunContext &context) {
            auto p_scene = context.properties.get("scene").get<dynasma::FirmPtr<Scene>>();
            auto p_windowFrame = context.properties.get(StandardCompositorOutputNames::OUTPUT)
                                     .get<dynasma::FirmPtr<FrameStore>>();
            context.properties.set("mat_shadow_view",
                                   p_scene->light.getViewMatrix(p_scene->camera));
            context.properties.set("mat_shadow_persp", p_scene->light.getProjectionMatrix());
            context.properties.set("light_direction", p_scene->light.direction);
            context.properties.set("light_color_primary", p_scene->light.color_primary);
            context.properties.set("light_color_ambient", p_scene->light.color_ambient);
        }});

    // camera matrices extractor
    auto p_extractCameraProperties =
        dynasma::makeStandalone<ComposeFunction>(ComposeFunction::SetupParams{
            .inputSpecs = {{PropertySpec{
                .name = "scene", .typeInfo = Variant::getTypeInfo<dynasma::FirmPtr<Scene>>()}}},
            .outputSpecs = {{
                PropertySpec{.name = "camera_view", .typeInfo = Variant::getTypeInfo<glm::mat4>()},
                PropertySpec{.name = "camera_persp", .typeInfo = Variant::getTypeInfo<glm::mat4>()},
                PropertySpec{.name = "camera_position",
                             .typeInfo = Variant::getTypeInfo<glm::vec3>()},
            }},
            .p_function = [](const RenderRunContext &context) {
                auto p_scene = context.properties.get("scene").get<dynasma::FirmPtr<Scene>>();
                auto p_windowFrame = context.properties.get(StandardCompositorOutputNames::OUTPUT)
                                         .get<dynasma::FirmPtr<FrameStore>>();
                context.properties.set("camera_view", p_scene->camera.getViewMatrix());
                context.properties.set("camera_persp",
                                       p_scene->camera.getPerspectiveMatrix(
                                           p_windowFrame->getSize().x, p_windowFrame->getSize().y));
                context.properties.set("camera_position", p_scene->camera.position);
            }});

    // shadow render

    auto p_shadowClear = root.getComponent<ComposeClearRenderKeeper>().new_asset(
        {ComposeClearRender::SetupParams{.root = root,
                                         .backgroundColor = glm::vec4(0.0f, 0.5f, 0.8f, 1.0f),
                                         .displayOutputPropertyName = "shadow_cleared"}});

    auto p_shadowRender =
        root.getComponent<ComposeSceneRenderKeeper>().new_asset({ComposeSceneRender::SetupParams{
            .root = root,
            .viewInputPropertyName = "mat_shadow_view",
            .perspectiveInputPropertyName = "mat_shadow_persp",
            .displayInputPropertyName = "shadow_cleared",
            .displayOutputPropertyName = "rendered_shadow",
            .cullingMode = ComposeSceneRender::CullingMode::Frontface}});

    auto p_shadowTexture = dynasma::makeStandalone<ComposeFrameToTexture>(
        ComposeFrameToTexture::SetupParams{.root = root,
                                           .frameInputPropertyName = "rendered_shadow",
                                           .colorTextureOutputPropertyName = "",
                                           .depthTextureOutputPropertyName = "tex_shadow",
                                           .size = {2048, 2048},
                                           .horWrap = Texture::WrappingType::BORDER_COLOR,
                                           .verWrap = Texture::WrappingType::BORDER_COLOR,
                                           .minFilter = Texture::FilterType::NEAREST,
                                           .magFilter = Texture::FilterType::NEAREST,
                                           .useMipMaps = false});

    // normal render

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
            .displayOutputPropertyName = StandardCompositorOutputNames::OUTPUT,
            .customInputPropertyNames = {
                PropertySpec{.name = "tex_shadow",
                             .typeInfo = Variant::getTypeInfo<dynasma::FirmPtr<Texture>>()},
                PropertySpec{.name = "mat_shadow_view",
                             .typeInfo = Variant::getTypeInfo<glm::mat4>()},
                PropertySpec{.name = "mat_shadow_persp",
                             .typeInfo = Variant::getTypeInfo<glm::mat4>()},
                PropertySpec{.name = "camera_position",
                             .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                PropertySpec{.name = "light_direction",
                             .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                PropertySpec{.name = "light_color_primary",
                             .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                PropertySpec{.name = "light_color_ambient",
                             .typeInfo = Variant::getTypeInfo<glm::vec3>()}}}});

    p_composeMethod =
        dynasma::makeStandalone<Method<ComposeTask>>(Method<ComposeTask>::MethodParams{
            .tasks = {p_extractLightProperties, p_extractCameraProperties, p_shadowClear,
                      p_shadowRender, p_shadowTexture, p_clear, p_normalRender}});
}
