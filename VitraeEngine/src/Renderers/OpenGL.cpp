#include "Vitrae/Renderers/OpenGL.h"
#include "Vitrae/Renderers/OpenGL/Mesh.h"
#include "Vitrae/Renderers/OpenGL/Texture.h"

#include "dynasma/keepers/naive.hpp"
#include "dynasma/managers/basic.hpp"
namespace Vitrae
{
    OpenGLRenderer::OpenGLRenderer():
        mBufferSpecCtr(0)
    {
        auto &posSpec = createVertexBufferSpec<aiVector3D>();
        posSpec.srcGetter = [](const aiMesh& extMesh) -> const aiVector3D* {
            if (extMesh.HasPositions()) {
                return extMesh.mVertices;
            } else {
                return nullptr;
            }
        };
        posSpec.name = "position";
        posSpec.vertexStorage = &Vertex::pos;

        auto &normSpec = createVertexBufferSpec<aiVector3D>();
        normSpec.srcGetter = [](const aiMesh& extMesh) -> const aiVector3D* {
            if (extMesh.HasNormals()) {
                return extMesh.mNormals;
            } else {
                return nullptr;
            }
        };
        posSpec.name = "normal";
        normSpec.vertexStorage = &Vertex::normal;

        auto &uvwSpec = createVertexBufferSpec<aiVector3D>();
        uvwSpec.srcGetter = [](const aiMesh& extMesh) -> const aiVector3D* {
            if (extMesh.HasTextureCoords(0)) {
                return extMesh.mTextureCoords[0];
            } else {
                return nullptr;
            }
        };
        posSpec.name = "textureCoord0";
        uvwSpec.vertexStorage = &Vertex::uvw;

        auto &colorSpec = createVertexBufferSpec<aiColor4D>();
        colorSpec.srcGetter = [](const aiMesh& extMesh) -> const aiColor4D* {
            if (extMesh.HasVertexColors(0)) {
                return extMesh.mColors[0];
            } else {
                return nullptr;
            }
        };
        posSpec.name = "color0";
        colorSpec.vertexStorage = &Vertex::col;
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
        return Unique<MeshKeeper>(new dynasma::NaiveKeeper<ImmediateMeshSeed, std::allocator<OpenGLMesh>>());
    }

    Unique<TextureManager> OpenGLRenderer::newTextureManager()
    {
        return Unique<TextureManager>(new dynasma::BasicManager<TextureSeed, std::allocator<OpenGLTexture>>());
    }
}