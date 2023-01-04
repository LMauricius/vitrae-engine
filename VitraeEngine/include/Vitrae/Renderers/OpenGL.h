#include "Vitrae/Renderer.h"

namespace Vitrae
{
    
    class OpenGLRenderer
    {
    public:
        OpenGLRenderer();

        void setup();
        void free();
        void render();
        
        Unique<Mesh> newMesh();
    };

}