#pragma once

#include "Vitrae/Renderer.h"
#include "Vitrae/Util/GraphicPrimitives.h"
#include "Vitrae/Util/AssimpTypeConvert.h"
#include "Vitrae/Util/VectorInfo.h"
#include "Vitrae/Util/GLTypeInfo.h"
#include "assimp/mesh.h"

#include <map>
#include <vector>
#include <functional>

namespace Vitrae
{
    class Mesh;
    class Material;

    template<class AssimpT>
    struct VertexBufferSpec
    {
        using aiType = AssimpT;
        using glmType = typename aiTypeCvt<AssimpT>::glmType;
        static constexpr GLint NumComponents = VectorInfo<glmType>::NumComponents;
        static constexpr GLenum ComponentTypeId = GLTypeInfo<typename VectorInfo<glmType>::value_type>::GlTypeId;

        // get and store

        /**
         * @returns a pointer to array of data from an aiMesh,
         * or nullptr if data cannot be found
        */
        std::function<const AssimpT*(const aiMesh&)> srcGetter;

        /**
         * Pointer to the field inside a Vertex struct,
         * or nullptr if there is no equivalent in Vertex
        */
        glmType Vertex::*vertexStorage;

        /**
         * The name of the buffer
        */
        String name;

        // shader info
        std::size_t layoutInd;
    };

    class OpenGLRenderer: public Renderer
    {
    public:
        OpenGLRenderer();
        ~OpenGLRenderer();

        void setup();
        void free();
        void render();
        
        Unique<ResourceManager<Mesh>> newMeshManager();
        Unique<ResourceManager<Material>> newMaterialManager();

        template<class T>
        const std::vector<VertexBufferSpec<T>> &getVertexBufferSpecs() const
        {
            throw std::runtime_error("Cannot get buffer specs of desired type T!");
        }
        template<class T>
        std::vector<VertexBufferSpec<T>> &getVertexBufferSpecs()
        {
            throw std::runtime_error("Cannot get buffer specs of desired type T!");
        }
        inline std::size_t getNumVertexBuffers() const
        {
            return mBufferSpecCtr;
        }

        template<class T>
        VertexBufferSpec<T> &createVertexBufferSpec()
        {
            auto &specs = getVertexBufferSpecs<T>();

            specs.emplace_back();
            specs.back().layoutInd = mBufferSpecCtr;

            mBufferSpecCtr++;

            return specs.back();
        }

    protected:
        std::vector<VertexBufferSpec<aiVector2D>> mVec2BufferSpecs;
        std::vector<VertexBufferSpec<aiVector3D>> mVec3BufferSpecs;
        std::vector<VertexBufferSpec<aiColor3D >> mCol3BufferSpecs;
        std::vector<VertexBufferSpec<aiColor4D >> mCol4BufferSpecs;
        std::size_t mBufferSpecCtr;
    };

    template<> const std::vector<VertexBufferSpec<aiVector2D>> &OpenGLRenderer::getVertexBufferSpecs<aiVector2D>() const { return mVec2BufferSpecs; }
    template<> const std::vector<VertexBufferSpec<aiVector3D>> &OpenGLRenderer::getVertexBufferSpecs<aiVector3D>() const { return mVec3BufferSpecs; }
    template<> const std::vector<VertexBufferSpec<aiColor3D >> &OpenGLRenderer::getVertexBufferSpecs<aiColor3D >() const { return mCol3BufferSpecs; }
    template<> const std::vector<VertexBufferSpec<aiColor4D >> &OpenGLRenderer::getVertexBufferSpecs<aiColor4D >() const { return mCol4BufferSpecs; }

    template<> std::vector<VertexBufferSpec<aiVector2D>> &OpenGLRenderer::getVertexBufferSpecs<aiVector2D>() { return mVec2BufferSpecs; }
    template<> std::vector<VertexBufferSpec<aiVector3D>> &OpenGLRenderer::getVertexBufferSpecs<aiVector3D>() { return mVec3BufferSpecs; }
    template<> std::vector<VertexBufferSpec<aiColor3D >> &OpenGLRenderer::getVertexBufferSpecs<aiColor3D >() { return mCol3BufferSpecs; }
    template<> std::vector<VertexBufferSpec<aiColor4D >> &OpenGLRenderer::getVertexBufferSpecs<aiColor4D >() { return mCol4BufferSpecs; }
}