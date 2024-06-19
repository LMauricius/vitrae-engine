#pragma once

#include "Vitrae/Assets/Mesh.hpp"
#include "Vitrae/Types/Typedefs.hpp"
#include "Vitrae/Util/StringId.hpp"

#include "glad/glad.h"

#include <map>
#include <optional>
#include <valarray>

namespace Vitrae
{
class OpenGLRenderer;

class OpenGLMesh : public Mesh
{
  public:
    OpenGLMesh();
    OpenGLMesh(const AssimpLoadParams &params);
    ~OpenGLMesh();

    void loadToGPU(OpenGLRenderer &rend);
    void unloadFromGPU();

    void setMaterial(dynasma::LazyPtr<Material> mat) override;
    dynasma::LazyPtr<Material> getMaterial() const override;
    std::span<const Triangle> getTriangles() const override;

    inline std::size_t memory_cost() const override
    {
        /// TODO: compute the real cost
        return sizeof(*this);
    }

    GLuint VAO;
    std::vector<GLuint> VBOs;
    GLuint EBO;

  protected:
    std::optional<dynasma::LazyPtr<Material>> mMaterial;
    std::vector<Triangle> mTriangles;
    std::map<StringId, std::valarray<glm::vec1>> namedVec1Buffers;
    std::map<StringId, std::valarray<glm::vec2>> namedVec2Buffers;
    std::map<StringId, std::valarray<glm::vec3>> namedVec3Buffers;
    std::map<StringId, std::valarray<glm::vec4>> namedVec4Buffers;

    bool m_sentToGPU;
};
} // namespace Vitrae