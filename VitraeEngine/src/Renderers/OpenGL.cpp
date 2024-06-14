#include "Vitrae/Renderers/OpenGL.hpp"
#include "Vitrae/ComponentRoot.hpp"
#include "Vitrae/Renderers/OpenGL/Compositing/SceneRender.hpp"
#include "Vitrae/Renderers/OpenGL/FrameStore.hpp"
#include "Vitrae/Renderers/OpenGL/Mesh.hpp"
#include "Vitrae/Renderers/OpenGL/ShaderCompilation.hpp"
#include "Vitrae/Renderers/OpenGL/Shading/Constant.hpp"
#include "Vitrae/Renderers/OpenGL/Shading/Function.hpp"
#include "Vitrae/Renderers/OpenGL/SharedBuffer.hpp"
#include "Vitrae/Renderers/OpenGL/Texture.hpp"

#include "dynasma/cachers/basic.hpp"
#include "dynasma/keepers/naive.hpp"
#include "dynasma/managers/basic.hpp"

#include "glad/glad.h"
// must be after glad.h
#include "GLFW/glfw3.h"

namespace Vitrae
{
OpenGLRenderer::OpenGLRenderer()
{
    /*
    Standard GLSL ypes
    */
    // clang-format off
    specifyGlType({.glTypeName = "float",  .std140Size = 4,  .std140Alignment = 4,  .layoutIndexSize = 1});
    specifyGlType({.glTypeName = "vec2",   .std140Size = 8,  .std140Alignment = 8,  .layoutIndexSize = 1});
    specifyGlType({.glTypeName = "vec3",   .std140Size = 12, .std140Alignment = 16, .layoutIndexSize = 1});
    specifyGlType({.glTypeName = "vec4",   .std140Size = 16, .std140Alignment = 16, .layoutIndexSize = 1});
    specifyGlType({.glTypeName = "mat2",   .std140Size = 32, .std140Alignment = 16, .layoutIndexSize = 2});
    specifyGlType({.glTypeName = "mat2x3", .std140Size = 48, .std140Alignment = 16, .layoutIndexSize = 2});
    specifyGlType({.glTypeName = "mat2x4", .std140Size = 64, .std140Alignment = 16, .layoutIndexSize = 2});
    specifyGlType({.glTypeName = "mat3",   .std140Size = 48, .std140Alignment = 16, .layoutIndexSize = 3});
    specifyGlType({.glTypeName = "mat3x2", .std140Size = 32, .std140Alignment = 8,  .layoutIndexSize = 3});
    specifyGlType({.glTypeName = "mat3x4", .std140Size = 64, .std140Alignment = 16, .layoutIndexSize = 3});
    specifyGlType({.glTypeName = "mat4",   .std140Size = 64, .std140Alignment = 16, .layoutIndexSize = 4});
    specifyGlType({.glTypeName = "mat4x2", .std140Size = 32, .std140Alignment = 8,  .layoutIndexSize = 4});
    specifyGlType({.glTypeName = "mat4x3", .std140Size = 48, .std140Alignment = 16, .layoutIndexSize = 4});
    specifyGlType({.glTypeName = "int",    .std140Size = 4,  .std140Alignment = 4,  .layoutIndexSize = 1});
    specifyGlType({.glTypeName = "ivec2",  .std140Size = 8,  .std140Alignment = 8,  .layoutIndexSize = 1});
    specifyGlType({.glTypeName = "ivec3",  .std140Size = 12, .std140Alignment = 16, .layoutIndexSize = 1});
    specifyGlType({.glTypeName = "ivec4",  .std140Size = 16, .std140Alignment = 16, .layoutIndexSize = 1});
    specifyGlType({.glTypeName = "uint",   .std140Size = 4,  .std140Alignment = 4,  .layoutIndexSize = 1});
    specifyGlType({.glTypeName = "uvec2",  .std140Size = 8,  .std140Alignment = 8,  .layoutIndexSize = 1});
    specifyGlType({.glTypeName = "uvec3",  .std140Size = 12, .std140Alignment = 16, .layoutIndexSize = 1});
    specifyGlType({.glTypeName = "uvec4",  .std140Size = 16, .std140Alignment = 16, .layoutIndexSize = 1});
    specifyGlType({.glTypeName = "bool",   .std140Size = 4,  .std140Alignment = 4,  .layoutIndexSize = 1});
    specifyGlType({.glTypeName = "bvec2",  .std140Size = 8,  .std140Alignment = 8,  .layoutIndexSize = 1});
    specifyGlType({.glTypeName = "bvec3",  .std140Size = 12, .std140Alignment = 16, .layoutIndexSize = 1});
    specifyGlType({.glTypeName = "bvec4",  .std140Size = 16, .std140Alignment = 16, .layoutIndexSize = 1});

    specifyGlType({.glTypeName = "sampler2D", .std140Size = 0, .std140Alignment = 0, .layoutIndexSize = 1});

    /*
    Type conversions
    */
    specifyTypeConversion({.hostType = Variant::getTypeInfo<       float>(), .glTypeSpec = getGlTypeSpec("float"), .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                               glUniform1f(glUniformId, hostValue.get<float>()                                    );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<   glm::vec2>(), .glTypeSpec = getGlTypeSpec("vec2"),  .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                              glUniform2fv(glUniformId, 1, &hostValue.get<glm::vec2>()[0]                         );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<   glm::vec3>(), .glTypeSpec = getGlTypeSpec("vec3"),  .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                              glUniform3fv(glUniformId, 1, &hostValue.get<glm::vec3>()[0]                         );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<   glm::vec4>(), .glTypeSpec = getGlTypeSpec("vec4"),  .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                              glUniform4fv(glUniformId, 1, &hostValue.get<glm::vec4>()[0]                         );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<         int>(), .glTypeSpec = getGlTypeSpec("int"),   .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                               glUniform1i(glUniformId, hostValue.get<int>()                                      );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<  glm::ivec2>(), .glTypeSpec = getGlTypeSpec("ivec2"), .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                              glUniform2iv(glUniformId, 1, &hostValue.get<glm::ivec2>()[0]                        );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<  glm::ivec3>(), .glTypeSpec = getGlTypeSpec("ivec3"), .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                              glUniform3iv(glUniformId, 1, &hostValue.get<glm::ivec3>()[0]                        );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<  glm::ivec4>(), .glTypeSpec = getGlTypeSpec("ivec4"), .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                              glUniform4iv(glUniformId, 1, &hostValue.get<glm::ivec4>()[0]                        );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<unsigned int>(), .glTypeSpec = getGlTypeSpec("uint"),  .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                              glUniform1ui(glUniformId, hostValue.get<unsigned int>()                             );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<  glm::uvec2>(), .glTypeSpec = getGlTypeSpec("uvec2"), .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                             glUniform2uiv(glUniformId, 1, &hostValue.get<glm::uvec2>()[0]                        );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<  glm::uvec3>(), .glTypeSpec = getGlTypeSpec("uvec3"), .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                             glUniform3uiv(glUniformId, 1, &hostValue.get<glm::uvec3>()[0]                        );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<  glm::uvec4>(), .glTypeSpec = getGlTypeSpec("uvec4"), .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                             glUniform4uiv(glUniformId, 1, &hostValue.get<glm::uvec4>()[0]                        );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<        bool>(), .glTypeSpec = getGlTypeSpec("bool"),  .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                               glUniform1i(glUniformId, hostValue.get<bool>() ? 1 : 0                             );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<  glm::bvec2>(), .glTypeSpec = getGlTypeSpec("bvec2"), .setUniform = [](GLint glUniformId, const Variant &hostValue) { const glm::bvec2& val = hostValue.get<glm::bvec2>();          glUniform2i(glUniformId, val.x ? 1 : 0, val.y ? 1 : 0                              );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<  glm::bvec3>(), .glTypeSpec = getGlTypeSpec("bvec3"), .setUniform = [](GLint glUniformId, const Variant &hostValue) { const glm::bvec3& val = hostValue.get<glm::bvec3>();          glUniform3i(glUniformId, val.x ? 1 : 0, val.y ? 1 : 0, val.z ? 1 : 0               );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<  glm::bvec4>(), .glTypeSpec = getGlTypeSpec("bvec4"), .setUniform = [](GLint glUniformId, const Variant &hostValue) { const glm::bvec4& val = hostValue.get<glm::bvec4>();          glUniform4i(glUniformId, val.x ? 1 : 0, val.y ? 1 : 0, val.z ? 1 : 0, val.w ? 1 : 0);}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<   glm::mat2>(), .glTypeSpec = getGlTypeSpec("mat2"),  .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                        glUniformMatrix2fv(glUniformId, 1, GL_FALSE, &hostValue.get<glm::mat2>()[0][0]            );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo< glm::mat2x3>(), .glTypeSpec = getGlTypeSpec("mat2x3"),.setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                      glUniformMatrix2x3fv(glUniformId, 1, GL_FALSE, &hostValue.get<glm::mat2x3>()[0][0]          );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo< glm::mat2x4>(), .glTypeSpec = getGlTypeSpec("mat2x4"),.setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                      glUniformMatrix2x4fv(glUniformId, 1, GL_FALSE, &hostValue.get<glm::mat2x4>()[0][0]          );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<   glm::mat3>(), .glTypeSpec = getGlTypeSpec("mat3"),  .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                        glUniformMatrix3fv(glUniformId, 1, GL_FALSE, &hostValue.get<glm::mat3>()[0][0]            );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo< glm::mat3x2>(), .glTypeSpec = getGlTypeSpec("mat3x2"),.setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                      glUniformMatrix3x2fv(glUniformId, 1, GL_FALSE, &hostValue.get<glm::mat3x2>()[0][0]          );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo< glm::mat3x4>(), .glTypeSpec = getGlTypeSpec("mat3x4"),.setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                      glUniformMatrix3x4fv(glUniformId, 1, GL_FALSE, &hostValue.get<glm::mat3x4>()[0][0]          );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo<   glm::mat4>(), .glTypeSpec = getGlTypeSpec("mat4"),  .setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                        glUniformMatrix4fv(glUniformId, 1, GL_FALSE, &hostValue.get<glm::mat4>()[0][0]            );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo< glm::mat4x2>(), .glTypeSpec = getGlTypeSpec("mat4x2"),.setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                      glUniformMatrix4x2fv(glUniformId, 1, GL_FALSE, &hostValue.get<glm::mat4x2>()[0][0]          );}});
    specifyTypeConversion({.hostType = Variant::getTypeInfo< glm::mat4x3>(), .glTypeSpec = getGlTypeSpec("mat4x3"),.setUniform = [](GLint glUniformId, const Variant &hostValue) {                                                      glUniformMatrix4x3fv(glUniformId, 1, GL_FALSE, &hostValue.get<glm::mat4x3>()[0][0]          );}});
    // clang-format on

    specifyTypeConversion({.hostType = Variant::getTypeInfo<dynasma::FirmPtr<Texture>>(),
                           .glTypeSpec = getGlTypeSpec("sampler2D"),
                           .setBinding = [](int bindingIndex, const Variant &hostValue) {
                               OpenGLTexture &tex = static_cast<OpenGLTexture &>(
                                   *hostValue.get<dynasma::FirmPtr<Texture>>());
                               glActiveTexture(GL_TEXTURE0 + bindingIndex);
                               glBindTexture(GL_TEXTURE_2D, tex.glTextureId);
                           }});

    /*
    Mesh vertex buffers for standard components
    */
    specifyVertexBuffer({.name = StandardVertexBufferNames::POSITION,
                         .typeInfo = Variant::getTypeInfo<glm::vec3>()});
    specifyVertexBuffer(
        {.name = StandardVertexBufferNames::NORMAL, .typeInfo = Variant::getTypeInfo<glm::vec3>()});
    specifyVertexBuffer({.name = StandardVertexBufferNames::TEXTURE_COORD,
                         .typeInfo = Variant::getTypeInfo<glm::vec2>()});
    specifyVertexBuffer(
        {.name = StandardVertexBufferNames::COLOR, .typeInfo = Variant::getTypeInfo<glm::vec4>()});
}

OpenGLRenderer::~OpenGLRenderer() {}

void OpenGLRenderer::setup(ComponentRoot &root)
{
    // clang-format off
    root.setComponent<              MeshKeeper>(new  dynasma::NaiveKeeper<              MeshKeeperSeed, std::allocator<              OpenGLMesh>>());
    root.setComponent<          TextureManager>(new dynasma::BasicManager<                 TextureSeed, std::allocator<           OpenGLTexture>>());
    root.setComponent<       FrameStoreManager>(new dynasma::BasicManager<              FrameStoreSeed, std::allocator<        OpenGLFrameStore>>());
    root.setComponent<   RawSharedBufferKeeper>(new  dynasma::NaiveKeeper<   RawSharedBufferKeeperSeed, std::allocator<   OpenGLRawSharedBuffer>>());
    root.setComponent<    ShaderConstantKeeper>(new  dynasma::NaiveKeeper<    ShaderConstantKeeperSeed, std::allocator<    OpenGLShaderConstant>>());
    root.setComponent<    ShaderFunctionKeeper>(new  dynasma::NaiveKeeper<    ShaderFunctionKeeperSeed, std::allocator<    OpenGLShaderFunction>>());
    root.setComponent<ComposeSceneRenderKeeper>(new  dynasma::NaiveKeeper<ComposeSceneRenderKeeperSeed, std::allocator<OpenGLComposeSceneRender>>());
    root.setComponent<CompiledGLSLShaderCacher>(new  dynasma::BasicCacher<CompiledGLSLShaderCacherSeed, std::allocator<      CompiledGLSLShader>>());
    // clang-format on

    glfwInit();
    gladLoadGL();

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
}

void OpenGLRenderer::free()
{
    glfwTerminate();
}

void OpenGLRenderer::render() {}

void OpenGLRenderer::specifyGlType(const GLTypeSpec &newSpec)
{
    m_glTypes.emplace(StringId(newSpec.glTypeName), newSpec);
}

void OpenGLRenderer::specifyTypeConversion(const GLConversionSpec &newSpec)
{
    m_glConversions.emplace(std::type_index(*newSpec.hostType.p_id), newSpec);
}

const GLTypeSpec &OpenGLRenderer::getGlTypeSpec(StringId name) const
{
    return m_glTypes.at(name);
}

const GLConversionSpec &OpenGLRenderer::getTypeConversion(const TypeInfo &type) const
{
    return m_glConversions.at(std::type_index(*type.p_id));
}

const std::map<StringId, GLTypeSpec> &OpenGLRenderer::getAllGlTypeSpecs() const
{
    return m_glTypes;
}

void OpenGLRenderer::specifyVertexBuffer(const PropertySpec &newElSpec)
{
    const GLTypeSpec &glTypeSpec = getTypeConversion(newElSpec.typeInfo).glTypeSpec;

    m_vertexBufferIndices.emplace(StringId(newElSpec.name), m_vertexBufferFreeIndex);
    m_vertexBufferFreeIndex += glTypeSpec.layoutIndexSize;
    m_vertexBufferSpecs.emplace(StringId(newElSpec.name), glTypeSpec);

    const dynasma::LazyPtr<Task> a, b;

    bool r = a < b;
}

std::size_t OpenGLRenderer::getNumVertexBuffers() const
{
    return m_vertexBufferIndices.size();
}

std::size_t OpenGLRenderer::getVertexBufferLayoutIndex(StringId name) const
{
    return m_vertexBufferIndices.at(name);
}

const std::map<StringId, const GLTypeSpec &> &OpenGLRenderer::getAllVertexBufferSpecs() const
{
    return m_vertexBufferSpecs;
}

OpenGLRenderer::GpuValueStorageMethod OpenGLRenderer::getGpuStorageMethod(
    const GLTypeSpec &spec) const
{
    if (spec.glTypeName == "sampler2D") {
        return GpuValueStorageMethod::UniformBinding;
    } else if (spec.glslDefinitionSnippet.empty()) {
        return GpuValueStorageMethod::Uniform;
    } else if (spec.flexibleMemberSpec.has_value()) {
        return GpuValueStorageMethod::SSBO;
    } else {
        return GpuValueStorageMethod::UBO;
    }
}
} // namespace Vitrae
