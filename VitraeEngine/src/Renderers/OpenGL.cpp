#include "Vitrae/Renderers/OpenGL.hpp"
#include "Vitrae/ComponentRoot.hpp"
#include "Vitrae/Renderers/OpenGL/Mesh.hpp"
#include "Vitrae/Renderers/OpenGL/Shading/Constant.hpp"
#include "Vitrae/Renderers/OpenGL/Shading/Function.hpp"
#include "Vitrae/Renderers/OpenGL/SharedBuffer.hpp"
#include "Vitrae/Renderers/OpenGL/Texture.hpp"

#include "dynasma/keepers/naive.hpp"
#include "dynasma/managers/basic.hpp"

#include "glad/glad.h"
// must be after glad.h
#include "GLFW/glfw3.h"

namespace Vitrae
{
OpenGLRenderer::OpenGLRenderer()
{
    getVertexBufferLayoutIndex(StandardVertexBufferNames::POSITION);
    getVertexBufferLayoutIndex(StandardVertexBufferNames::NORMAL);
    getVertexBufferLayoutIndex(StandardVertexBufferNames::TEXTURE_COORD);
    getVertexBufferLayoutIndex(StandardVertexBufferNames::COLOR);
}

void OpenGLRenderer::setup(ComponentRoot &root)
{
    root.setComponent<MeshKeeper>(
        new dynasma::NaiveKeeper<MeshKeeperSeed, std::allocator<OpenGLMesh>>());
    root.setComponent<TextureManager>(
        new dynasma::BasicManager<TextureSeed, std::allocator<OpenGLTexture>>());
    root.setComponent<RawSharedBufferKeeper>(
        new dynasma::NaiveKeeper<RawSharedBufferKeeperSeed,
                                 std::allocator<OpenGLRawSharedBuffer>>());
    root.setComponent<ShaderConstantKeeper>(
        new dynasma::NaiveKeeper<ShaderConstantKeeperSeed, std::allocator<OpenGLShaderConstant>>());
    root.setComponent<ShaderFunctionKeeper>(
        new dynasma::NaiveKeeper<ShaderFunctionKeeperSeed, std::allocator<OpenGLShaderFunction>>());
}

void OpenGLRenderer::free() {}

void OpenGLRenderer::render() {}

const GLTypeSpec &OpenGLRenderer::specifyGlType(const GLTypeSpec &newSpec)
{
    return m_glTypes.emplace(StringId(newSpec.glTypeName), newSpec).first->second;
}

const GLConversionSpec &OpenGLRenderer::specifyTypeConversion(const GLConversionSpec &newSpec)
{
    return m_glConversions.emplace(std::type_index(*newSpec.hostType.p_id), newSpec).first->second;
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
    if (spec.glslDefinitionSnippet.empty()) {
        return GpuValueStorageMethod::Uniform;
    } else if (spec.flexibleMemberSpec.has_value()) {
        return GpuValueStorageMethod::SSBO;
    } else {
        return GpuValueStorageMethod::UBO;
    }
}
} // namespace Vitrae
