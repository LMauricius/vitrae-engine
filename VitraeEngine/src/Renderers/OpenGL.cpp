#include "Vitrae/Renderers/OpenGL.h"
#include "Vitrae/Renderers/OpenGL/Mesh.h"
#include "Vitrae/Renderers/OpenGL/Texture.h"
#include "Vitrae/ComponentRoot.h"

#include "OpenGL.h"
#include "dynasma/keepers/naive.hpp"
#include "dynasma/managers/basic.hpp"
namespace Vitrae
{
OpenGLRenderer::OpenGLRenderer()
{
    getVertexBufferLayoutIndex(StandardVertexBufferNames::POSITION);
    getVertexBufferLayoutIndex(StandardVertexBufferNames::NORMAL);
    getVertexBufferLayoutIndex(StandardVertexBufferNames::TEXTURE_COORD);
    getVertexBufferLayoutIndex(StandardVertexBufferNames::COLOR);
}

void OpenGLRenderer::setup(ComponentRoot& root)
{
    root.setComponent<MeshKeeper>(Unique<MeshKeeper>(
        new dynasma::NaiveKeeper<MeshKeeperSeed, std::allocator<OpenGLMesh>>()));
    root.setComponent<TextureManager>(Unique<TextureManager>(
        new dynasma::BasicManager<TextureSeed, std::allocator<OpenGLTexture>>()));
}

void OpenGLRenderer::free()
{
}

void OpenGLRenderer::render()
{
}

void OpenGLRenderer::specifyGlType(const GLTypeSpec &newSpec)
{
    m_glTypes[newSpec.name] = newSpec;
    m_glTypesByTypeIndex[std::type_index(*newSpec.p_type->p_id)] = &m_glTypes[newSpec.name];
}

const GLTypeSpec &OpenGLRenderer::getGlTypeSpec(StringId name) const
{
    return m_glTypes.at(name);
}

const GLTypeSpec &OpenGLRenderer::getGlTypeSpec(const TypeInfo &type) const
{
    return m_glTypesByTypeIndex.at(std::type_index(*newSpec.p_type->p_id));
}

const std::map<StringId, GLTypeSpec> &OpenGLRenderer::getAllGlTypeSpecs() const
{
    return m_glTypes;
}

void OpenGLRenderer::specifyVertexBuffer(const GLTypeSpec &newElSpec)
{
    m_vertexBufferIndices.emplace(StringId(newElSpec.name), m_vertexBufferFreeIndex);
    m_vertexBufferFreeIndex += newElSpec.layoutIndexSize;
    m_vertexBufferSpecs.emplace(StringId(newElSpec.name), newElSpec);
}

std::size_t OpenGLRenderer::getNumVertexBuffers() const
{
    return m_vertexBufferIndices.size();
}

std::size_t OpenGLRenderer::getVertexBufferLayoutIndex(StringId name) const
{
    return m_vertexBufferIndices.at(name);
}

const std::map<StringId, GLTypeSpec> &OpenGLRenderer::getAllVertexBufferSpecs() const
{
    return m_vertexBufferSpecs;
}

GpuValueStorageMethod OpenGLRenderer::getGpuStorageMethod(const GLTypeSpec &spec) const
{
    if (spec.glslDefinitionSnippet.empty())
    {
        return GpuValueStorageMethod::Uniform;
    }
    else if (spec.flexibleMemberSpec.has_value())
    {
        return GpuValueStorageMethod::SSBO;
    }
    else
    {
        return GpuValueStorageMethod::UBO;
    }
}
} // namespace Vitrae
