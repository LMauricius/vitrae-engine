#include "Vitrae/Mesh.h"

namespace Vitrae
{
    
    class OpenGLMesh : public Mesh
    {
    public:
        OpenGLMesh();
        ~OpenGLMesh();

        load(const aiMesh& extMesh);
        
        GLuint VAO;
        GLuint VBO_vrhovi, VBO_boje, VBO_normale, VBO_uv, VBO_transformacije;
        GLuint EBO;

    };

}