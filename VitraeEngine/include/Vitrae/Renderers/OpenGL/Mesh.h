#pragma once

#include "Vitrae/Util/Types.h"
#include "Vitrae/Assets/Mesh.h"

#include "glad/glad.h"

#include <vector>
#include <map>
#include <optional>

namespace Vitrae
{
    class OpenGLRenderer;

    class OpenGLMesh : public Mesh
    {
    public:
        OpenGLMesh(const AssimpLoadParams &params);
        ~OpenGLMesh();

        void loadToGPU(OpenGLRenderer & rend);
        void unloadFromGPU(OpenGLRenderer & rend);

        void setMaterial(dynasma::LazyPtr<Material> mat);
        dynasma::LazyPtr<Material> getMaterial() const;
        virtual std::span<const Vertex> getVertices() const;
        virtual std::span<const Triangle> getTriangles() const;

        GLuint VAO;
        std::vector<GLuint> VBOs;
        std::map<String, std::vector<glm::vec1>> namedVec1Buffers;
        std::map<String, std::vector<glm::vec2>> namedVec2Buffers;
        std::map<String, std::vector<glm::vec3>> namedVec3Buffers;
        std::map<String, std::vector<glm::vec4>> namedVec4Buffers;
        GLuint EBO;

    protected:
        std::optional<dynasma::LazyPtr<Material>> mMaterial;
        std::vector<Vertex> mVertices;
        std::vector<Triangle> mTriangles;
    };

}