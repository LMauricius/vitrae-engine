#include <iostream>

#include "assetCollection.hpp"

#include "Vitrae/Renderers/OpenGL.hpp"

using namespace Vitrae;

int main(int argc, char **argv)
{
    std::cout << "Hello, world!\n";

    String path = argv[1];
    float sceneScale = 1.0f;
    if (argc > 2) {
        sceneScale = std::stof(argv[2]);
    }

    /*
    Setup the system
    */

    ComponentRoot root;
    Renderer *p_rend(new OpenGLRenderer());
    root.setComponent<Renderer>(p_rend);
    p_rend->setup(root);

    /*
    Assets
    */
    AssetCollection collection(root, *p_rend, path, sceneScale);

    /*
    Render loop!
    */

    while (collection.running) {
        collection.render();
    }

    /*
    Free resources
    */
    p_rend->free();

    return 0;
}
