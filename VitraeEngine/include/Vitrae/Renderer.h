#include "Types.h"
#include "Mesh.h"

namespace Vitrae
{
    
    class Renderer
    {
    public:
        virtual ~Renderer() = 0;

        virtual void setup() = 0;
        virtual void free() = 0;
        virtual void render() = 0;
        
        virtual Unique<Mesh> newMesh() = 0;
    };

}