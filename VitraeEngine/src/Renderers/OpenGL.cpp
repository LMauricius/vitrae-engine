#include "Vitrae/Renderers/OpenGL.h"
#include "Vitrae/Renderers/OpenGL/Mesh.h"
#include "Vitrae/Renderers/OpenGL/Texture.h"
#include "Vitrae/ComponentRoot.h"

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

std::size_t OpenGLRenderer::getNumVertexBuffers() const
{
    return mVertexBufferIndices.size();
}

std::size_t OpenGLRenderer::getVertexBufferLayoutIndex(StringId name)
{
    return mVertexBufferIndices.emplace(name, mVertexBufferIndices.size()).first->second;
}
}