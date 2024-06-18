#include "methodCollection.hpp"

#include "Vitrae/Assets/Mesh.hpp"
#include "Vitrae/Visuals/Scene.hpp"

#include <set>

MethodCollection::MethodCollection(ComponentRoot &root) : root(root) {}

void MethodCollection::apply(Compositor &comp)
{
    comp.setComposeMethod(p_composeMethod);
    comp.setDefaultShadingMethod(p_vertexMethod, p_fragmentMethod);
}
