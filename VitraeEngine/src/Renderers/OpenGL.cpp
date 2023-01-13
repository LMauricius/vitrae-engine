#include "Vitrae/Renderers/OpenGL.h"
#include "Vitrae/Renderers/OpenGL/Mesh.h"
#include "Vitrae/Renderers/OpenGL/Texture.h"

#include "Vitrae/ResourceManagers/Simple.h"

namespace Vitrae
{

    namespace
    {
        struct OpenGLMeshLoader: public std::allocator<OpenGLMesh>
        {
            OpenGLRenderer *rend;

            inline OpenGLMeshLoader(OpenGLRenderer *rend):
                rend(rend)
            {

            }

            inline void setup(OpenGLMesh *m, const Mesh::SetupParams &params)
            {
                m->load(params, *rend);
            }

            inline void load(OpenGLMesh *m, const Mesh::LoadParams &params)
            {
                m->loadToGPU(*rend);
            }

            inline void unload(OpenGLMesh *m, const Mesh::LoadParams &params)
            {
                m->unloadFromGPU(*rend);
            }
        };

        struct OpenGLTextureLoader: public std::allocator<OpenGLTexture>
        {
            OpenGLRenderer *rend;

            inline OpenGLTextureLoader(OpenGLRenderer *rend):
                rend(rend)
            {

            }

            inline void setup(OpenGLTexture *t, const Texture::SetupParams &params)
            {
            }

            inline void load(OpenGLTexture *t, const Texture::LoadParams &params)
            {
                t->load(params, *rend);
                t->loadToGPU(*rend);
            }

            inline void unload(OpenGLTexture *t, const Texture::LoadParams &params)
            {
                t->unload();
                t->unloadFromGPU(*rend);
            }
        };
    }

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

    Unique<ResourceManager<Mesh>> OpenGLRenderer::newMeshManager()
    {
        return Unique<ResourceManager<Mesh>>(
            new SimpleResourceManager<Mesh, OpenGLMeshLoader>(OpenGLMeshLoader(this))
        );
    }

    Unique<ResourceManager<Texture>> OpenGLRenderer::newTextureManager()
    {
        return Unique<ResourceManager<Texture>>(
            new SimpleResourceManager<Texture, OpenGLTextureLoader>(OpenGLTextureLoader(this))
        );
    }


}