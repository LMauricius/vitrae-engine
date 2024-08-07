#pragma once

#include "../shadingModes.hpp"
#include "abstract.hpp"

#include "Vitrae/Pipelines/Compositing/ClearRender.hpp"
#include "Vitrae/Pipelines/Compositing/FrameToTexture.hpp"
#include "Vitrae/Pipelines/Compositing/Function.hpp"
#include "Vitrae/Pipelines/Compositing/SceneRender.hpp"
#include "Vitrae/Pipelines/Shading/Constant.hpp"
#include "Vitrae/Pipelines/Shading/Function.hpp"

#include "dynasma/standalone.hpp"

#include <iostream>

using namespace Vitrae;

// "Edge Bilinear Range"
struct MethodsShadowEBR : MethodCollection
{
    inline MethodsShadowEBR(ComponentRoot &root) : MethodCollection(root)
    {
        /*
        VERTEX SHADING
        */
        p_vertexMethod = dynasma::makeStandalone<Method<ShaderTask>>(
            Method<ShaderTask>::MethodParams{.tasks = {}, .friendlyName = "ShadowEBR"});

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
                    float inShadowTexel(sampler2D tex_shadow, ivec2 pos, float posz, float offset) {
                        if (texelFetch(tex_shadow, pos, 0).r < posz + offset) {
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
                        float offset = 0.5 / shadowSize.x;
                        vec2 texelPos = position_shadow.xy * shadowSize;
                        vec2 topLeftTexelPos = floor(position_shadow.xy * shadowSize);
                        ivec2 topLeftTexelPosI = ivec2(topLeftTexelPos);
                        vec2 bilinOffset = texelPos - topLeftTexelPos;

                        light_shadow_factor = (
                            inShadowTexel(
                                tex_shadow, topLeftTexelPosI + ivec2(0,0), position_shadow.z, offset
                            ) * (1.0 - bilinOffset.x) + inShadowTexel(
                                tex_shadow, topLeftTexelPosI + ivec2(1,0), position_shadow.z, offset
                            ) * bilinOffset.x
                        ) * (1.0 - bilinOffset.y) + (
                            inShadowTexel(
                                tex_shadow, topLeftTexelPosI + ivec2(0,1), position_shadow.z, offset
                            ) * (1.0 - bilinOffset.x) + inShadowTexel(
                                tex_shadow, topLeftTexelPosI + ivec2(1,1), position_shadow.z, offset
                            ) * bilinOffset.x
                        ) * bilinOffset.y;
                        
                        if (light_shadow_factor >= 0.5) {
                            light_shadow_factor = 1.0;
                        }
                        else {
                            light_shadow_factor = 0.0;
                        }
                    }
                )",
                .functionName = "lightShadowFactor"}});

        p_fragmentMethod =
            dynasma::makeStandalone<Method<ShaderTask>>(Method<ShaderTask>::MethodParams{
                .tasks = {p_shadowLightFactor}, .friendlyName = "ShadowEBR"});

        /*
        COMPOSING
        */

        // shadow matrices extractor
        auto p_extractLightProperties =
            dynasma::makeStandalone<ComposeFunction>(ComposeFunction::SetupParams{
                .inputSpecs = {{PropertySpec{
                    .name = "scene", .typeInfo = Variant::getTypeInfo<dynasma::FirmPtr<Scene>>()}}},
                .outputSpecs = {{
                    PropertySpec{.name = "mat_shadow_view",
                                 .typeInfo = Variant::getTypeInfo<glm::mat4>()},
                    PropertySpec{.name = "mat_shadow_persp",
                                 .typeInfo = Variant::getTypeInfo<glm::mat4>()},
                    PropertySpec{.name = "light_direction",
                                 .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                    PropertySpec{.name = "light_color_primary",
                                 .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                    PropertySpec{.name = "light_color_ambient",
                                 .typeInfo = Variant::getTypeInfo<glm::vec3>()},
                }},
                .p_function = [](const RenderRunContext &context) {
                    auto p_scene = context.properties.get("scene").get<dynasma::FirmPtr<Scene>>();
                    auto p_shadowFrame =
                        context.preparedCompositorFrameStores.at("rendered_shadow");
                    context.properties.set("mat_shadow_view",
                                           p_scene->light.getViewMatrix(
                                               p_scene->camera, 1.0 / p_shadowFrame->getSize().x));
                    context.properties.set("mat_shadow_persp",
                                           p_scene->light.getProjectionMatrix());
                    context.properties.set("light_direction",
                                           glm::normalize(p_scene->light.direction));
                    context.properties.set("light_color_primary", p_scene->light.color_primary);
                    context.properties.set("light_color_ambient", p_scene->light.color_ambient);
                }});

        // shadow render

        auto p_shadowClear = root.getComponent<ComposeClearRenderKeeper>().new_asset(
            {ComposeClearRender::SetupParams{.root = root,
                                             .backgroundColor = glm::vec4(0.0f, 0.5f, 0.8f, 1.0f),
                                             .displayOutputPropertyName = "shadow_cleared"}});

        auto p_shadowRender = root.getComponent<ComposeSceneRenderKeeper>().new_asset(
            {ComposeSceneRender::SetupParams{.root = root,
                                             .viewInputPropertyName = "mat_shadow_view",
                                             .perspectiveInputPropertyName = "mat_shadow_persp",
                                             .displayInputPropertyName = "shadow_cleared",
                                             .displayOutputPropertyName = "rendered_shadow",
                                             .cullingMode =
                                                 ComposeSceneRender::CullingMode::Frontface}});

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

        // compose method
        p_composeMethod =
            dynasma::makeStandalone<Method<ComposeTask>>(Method<ComposeTask>::MethodParams{
                .tasks = {p_extractLightProperties, p_shadowClear, p_shadowRender, p_shadowTexture},
                .friendlyName = "ShadowEBR"});
    }
};