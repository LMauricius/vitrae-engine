#pragma once

#include "Vitrae/Renderer.h"
#include "Vitrae/TypeConversion/AssimpTypeConvert.h"
#include "Vitrae/TypeConversion/GLTypeInfo.h"
#include "Vitrae/TypeConversion/VectorTypeInfo.h"
#include "Vitrae/Types/GraphicPrimitives.h"
#include "Vitrae/Util/StringId.h"
#include "assimp/mesh.h"

#include <functional>
#include <map>
#include <vector>

namespace Vitrae
{
class Mesh;
class Texture;

template <class AssimpT> struct VertexBufferSpec
{
    using aiType = AssimpT;
    using glmType = typename aiTypeCvt<AssimpT>::glmType;
    static constexpr GLint NumComponents = VectorTypeInfo<glmType>::NumComponents;
    static constexpr GLenum ComponentTypeId =
        GLTypeInfo<typename VectorTypeInfo<glmType>::value_type>::GlTypeId;

    // get and store

    /**
     * @returns a pointer to array of data from an aiMesh,
     * or nullptr if data cannot be found
     */
    std::function<const aiType *(const aiMesh &extMesh)> getAIVertexData;

    /**
     * The name of the buffer
     */
    String name;

    // shader info
    std::size_t layoutInd;
};

class OpenGLRenderer : public Renderer
{
  public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    void setup(ComponentRoot& root) override;
    void free() override;
    void render() override;

    template <class T> const std::vector<VertexBufferSpec<T>> &getVertexBufferSpecs() const
    {
        throw std::runtime_error("Cannot get buffer specs of desired type T!");
    }
    template <class T> std::vector<VertexBufferSpec<T>> &getVertexBufferSpecs()
    {
        throw std::runtime_error("Cannot get buffer specs of desired type T!");
    }
    std::size_t getNumVertexBuffers() const;
    std::size_t getVertexBufferLayoutIndex(StringId name);

  protected:
    std::map<StringId, std::size_t> mVertexBufferIndices;
};

template <>
const std::vector<VertexBufferSpec<aiVector2D>> &OpenGLRenderer::getVertexBufferSpecs<aiVector2D>()
    const
{
    return mVec2BufferSpecs;
}
template <>
const std::vector<VertexBufferSpec<aiVector3D>> &OpenGLRenderer::getVertexBufferSpecs<aiVector3D>()
    const
{
    return mVec3BufferSpecs;
}
template <>
const std::vector<VertexBufferSpec<aiColor3D>> &OpenGLRenderer::getVertexBufferSpecs<aiColor3D>()
    const
{
    return mCol3BufferSpecs;
}
template <>
const std::vector<VertexBufferSpec<aiColor4D>> &OpenGLRenderer::getVertexBufferSpecs<aiColor4D>()
    const
{
    return mCol4BufferSpecs;
}

template <>
std::vector<VertexBufferSpec<aiVector2D>> &OpenGLRenderer::getVertexBufferSpecs<aiVector2D>()
{
    return mVec2BufferSpecs;
}
template <>
std::vector<VertexBufferSpec<aiVector3D>> &OpenGLRenderer::getVertexBufferSpecs<aiVector3D>()
{
    return mVec3BufferSpecs;
}
template <>
std::vector<VertexBufferSpec<aiColor3D>> &OpenGLRenderer::getVertexBufferSpecs<aiColor3D>()
{
    return mCol3BufferSpecs;
}
template <>
std::vector<VertexBufferSpec<aiColor4D>> &OpenGLRenderer::getVertexBufferSpecs<aiColor4D>()
{
    return mCol4BufferSpecs;
}
} // namespace Vitrae