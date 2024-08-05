#pragma once

#include "Vitrae/Pipelines/Task.hpp"
#include "Vitrae/Renderer.hpp"
#include "Vitrae/TypeConversion/AssimpTypeConvert.hpp"
#include "Vitrae/TypeConversion/GLTypeInfo.hpp"
#include "Vitrae/TypeConversion/VectorTypeInfo.hpp"
#include "Vitrae/Types/GraphicPrimitives.hpp"
#include "Vitrae/Util/StringId.hpp"
#include "assimp/mesh.h"

#include "glad/glad.h"
// must be after glad.h
#include "GLFW/glfw3.h"

#include <functional>
#include <map>
#include <optional>
#include <typeindex>
#include <vector>

namespace Vitrae
{
class Mesh;
class Texture;
class RawSharedBuffer;

struct GLTypeSpec
{
    String glMutableTypeName;
    String glConstTypeName;

    String glslDefinitionSnippet;
    std::vector<const GLTypeSpec *> memberTypeDependencies;

    std::size_t std140Size;
    std::size_t std140Alignment;
    std::size_t layoutIndexSize;

    // used only if the type is a struct and has a flexible array member
    struct FlexibleMemberSpec
    {
        const GLTypeSpec &elementGlTypeSpec;
        std::size_t maxNumElements;
    };
    std::optional<FlexibleMemberSpec> flexibleMemberSpec;
};

struct GLConversionSpec
{
    static void std140ToHostIdentity(const GLConversionSpec *spec, const void *src, void *dst);
    static void hostToStd140Identity(const GLConversionSpec *spec, const void *src, void *dst);
    static dynasma::FirmPtr<RawSharedBuffer> getSharedBufferRaw(const GLConversionSpec *spec,
                                                                const void *src);

    const TypeInfo &hostType;
    const GLTypeSpec &glTypeSpec;

    void (*setUniform)(GLint glUniformId, const Variant &hostValue) = nullptr;
    void (*setBinding)(int bindingIndex, const Variant &hostValue) = nullptr;

    // used only if the type has a flexible array member
    struct FlexibleMemberConversion
    {
        std::size_t (*getNumElements)(const void *src);
    };
    std::optional<FlexibleMemberConversion> flexibleMemberSpec;
};

class OpenGLRenderer : public Renderer
{
  public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    void setup(ComponentRoot &root) override;
    void free() override;
    void update() override;

    GLFWwindow *getWindow();

    void specifyGlType(const GLTypeSpec &newSpec);
    const GLTypeSpec &getGlTypeSpec(StringId glslName) const;
    void specifyTypeConversion(const GLConversionSpec &newSpec);
    const GLConversionSpec &getTypeConversion(const TypeInfo &hostType) const;
    const std::map<StringId, GLTypeSpec> &getAllGlTypeSpecs() const;

    void specifyVertexBuffer(const PropertySpec &newElSpec);
    template <class T> void specifyVertexBufferAuto()
    {
        specifyVertexBuffer(getTypeConversion(Variant::getTypeInfo<T>().p_id->name()));
    }
    std::size_t getNumVertexBuffers() const;
    std::size_t getVertexBufferLayoutIndex(StringId name) const;
    const std::map<StringId, const GLTypeSpec &> &getAllVertexBufferSpecs() const;

    enum class GpuValueStorageMethod {
        Uniform,
        UniformBinding,
        UBO,
        SSBO
    };
    GpuValueStorageMethod getGpuStorageMethod(const GLTypeSpec &spec) const;

  protected:
    GLFWwindow *mp_mainWindow;

    std::map<StringId, GLTypeSpec> m_glTypes;
    std::map<std::type_index, GLConversionSpec> m_glConversions;

    std::map<StringId, std::size_t> m_vertexBufferIndices;
    std::size_t m_vertexBufferFreeIndex;
    std::map<StringId, const GLTypeSpec &> m_vertexBufferSpecs;
};

} // namespace Vitrae