#include "Vitrae/Renderers/OpenGL.h"
#include "Vitrae/Renderers/OpenGL/Mesh.h"
#include "Vitrae/Renderers/OpenGL/Texture.h"

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

void OpenGLRenderer::setup()
{
}

void OpenGLRenderer::free()
{
}

void OpenGLRenderer::render()
{
}
Unique<MeshKeeper> OpenGLRenderer::newMeshManager()
{
    return Unique<MeshKeeper>(
        new dynasma::NaiveKeeper<MeshKeeperSeed, std::allocator<OpenGLMesh>>());
}

Unique<TextureManager> OpenGLRenderer::newTextureManager()
{
    return Unique<TextureManager>(
        new dynasma::BasicManager<TextureSeed, std::allocator<OpenGLTexture>>());
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