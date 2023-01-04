#include "assimp/mesh.h"

namespace Vitrae
{
    
    class Mesh
    {
    public:
        virtual ~Mesh() = 0;

        virtual void load(const aiMesh& extMesh) = 0;
    };

}