#pragma once

#include "Vitrae/Pipelines/Task.h"
#include "Vitrae/Renderer.h"
#include "Vitrae/TypeConversion/AssimpTypeConvert.h"
#include "Vitrae/TypeConversion/GLTypeInfo.h"
#include "Vitrae/TypeConversion/VectorTypeInfo.h"
#include "Vitrae/Types/GraphicPrimitives.h"
#include "Vitrae/Util/StringId.h"
#include "assimp/mesh.h"

#include <functional>
#include <map>
#include <optional>
#include <typeindex>
#include <vector>

namespace Vitrae
{
class Mesh;
class Texture;

struct GLTypeSpec
{
    TypeInfo *p_hostType;
    String glslName;

    String glslDefinitionSnippet;
    std::vector<StringId> memberTypeDependencies;

    std::size_t hostSize;
    std::size_t hostAlignment;
    std::size_t std140Size;
    std::size_t std140Alignment;
    std::size_t layoutIndexSize;

    void (*std140ToHost)(const void *src, void *dst);
    void (*hostToStd140)(const void *src, void *dst);

    // used only if the type has a flexible array member
    struct FlexibleMemberSpec
    {
        const GLTypeSpec &elementGlTypeSpec;
        std::size_t maxNumElements;
        std::size_t (*getNumElements)(const void *src);
    };
    std::optional<FlexibleMemberSpec> flexibleMemberSpec;
};

class OpenGLRenderer : public Renderer
{
  public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    void setup(ComponentRoot& root) override;
    void free() override;
    void render() override;

    void specifyGlType(const GLTypeSpec &newSpec);
    const GLTypeSpec &getGlTypeSpec(StringId name) const;
    const GLTypeSpec &getGlTypeSpec(const TypeInfo &type) const;
    const std::map<StringId, GLTypeSpec> &getAllGlTypeSpecs() const;

    void specifyVertexBuffer(const GLTypeSpec &newElSpec);
    template <class T> void specifyVertexBufferAuto()
    {
        specifyVertexBuffer(getGlTypeSpec(Property::getTypeInfo<T>().p_id->name()));
    }
    std::size_t getNumVertexBuffers() const;
    std::size_t getVertexBufferLayoutIndex(StringId name) const;
    const std::map<StringId, GLTypeSpec> &getAllVertexBufferSpecs() const;

    enum class GpuValueStorageMethod
    {
        Uniform,
        UBO,
        SSBO
    };
    GpuValueStorageMethod getGpuStorageMethod(const GLTypeSpec &spec) const;

  protected:
    std::map<StringId, GLTypeSpec> m_glTypes;
    std::map<std::type_index, GLTypeSpec *> m_glTypesByTypeIndex;

    std::map<StringId, std::size_t> m_vertexBufferIndices;
    std::size_t m_vertexBufferFreeIndex;
    std::map<StringId, GLTypeSpec> m_vertexBufferSpecs;
};

} // namespace Vitrae