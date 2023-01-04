#include "Vitrae/Renderers/OpenGL.h"
#include "Vitrae/Renderers/OpenGL/Mesh.h"

namespace Vitrae
{

    OpenGLRenderer::OpenGLRenderer()
    {

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

    Unique<Mesh> OpenGLRenderer::newMesh()
    {
        return Unique<Mesh>(new OpenGLMesh());
    }


}