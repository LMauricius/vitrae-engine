#pragma once

#include "Vitrae/Resources/Mesh.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
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
        OpenGLMesh();
        ~OpenGLMesh();

        void load(const SetupParams &params, OpenGLRenderer & rend);
        void loadToGPU(OpenGLRenderer & rend);
        void unloadFromGPU(OpenGLRenderer & rend);

        void setMaterial(resource_ptr<Material> mat);
        resource_ptr<Material> getMaterial() const;
        const std::vector<Vertex> &getVertices() const;
        const std::vector<Triangle> &getTriangles() const;
        
        GLuint VAO;
        std::vector<GLuint> VBOs;
        std::map<String, std::vector<glm::vec1>> namedVec1Buffers;
        std::map<String, std::vector<glm::vec2>> namedVec2Buffers;
        std::map<String, std::vector<glm::vec3>> namedVec3Buffers;
        std::map<String, std::vector<glm::vec4>> namedVec4Buffers;
        GLuint EBO;

    protected:
        std::optional<resource_ptr<Material>> mMaterial;
        std::vector<Vertex> mVertices;
        std::vector<Triangle> mTriangles;
    };

}