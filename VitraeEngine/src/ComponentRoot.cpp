#include "Vitrae/ComponentRoot.h"
#include "Vitrae/Assets/Mesh.h"

#include <iostream>

namespace Vitrae
{
ComponentRoot::ComponentRoot()
    : mInfoStream(&std::cout), mWarningStream(&std::cout), mErrStream(&std::cerr)
{
    addAiMeshBufferInfo<aiVector3D>(
        {StandardVertexBufferNames::POSITION, [](const aiMesh &extMesh) -> const aiVector3D * {
             if (extMesh.HasPositions()) {
                 return extMesh.mVertices;
             } else {
                 return nullptr;
             }
         }});

    addAiMeshBufferInfo<aiVector3D>(
        {StandardVertexBufferNames::NORMAL, [](const aiMesh &extMesh) -> const aiVector3D * {
             if (extMesh.HasNormals()) {
                 return extMesh.mNormals;
             } else {
                 return nullptr;
             }
         }});

    addAiMeshBufferInfo<aiVector3D>(
        {StandardVertexBufferNames::TEXTURE_COORD, [](const aiMesh &extMesh) -> const aiVector3D * {
             if (extMesh.HasTextureCoords(0)) {
                 return extMesh.mTextureCoords[0];
             } else {
                 return nullptr;
             }
         }});

    addAiMeshBufferInfo<aiColor4D>(
        {StandardVertexBufferNames::COLOR, [](const aiMesh &extMesh) -> const aiColor4D * {
             if (extMesh.HasVertexColors(0)) {
                 return extMesh.mColors[0];
             } else {
                 return nullptr;
             }
         }});
}

ComponentRoot::~ComponentRoot() {}
} // namespace Vitrae