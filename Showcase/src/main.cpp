#include <iostream>

#include "assetCollection.hpp"
#include "classicShading.hpp"

using namespace Vitrae;

int main(int argc, char **argv)
{
    std::cout << "Hello, world!\n";

    String path = argv[1];
    float sceneScale = 1.0f;
    if (argc > 2) {
        sceneScale = std::stof(argv[2]);
    }

    AssetCollection collection(path, sceneScale);

    ClassicShading shadingModel(collection.root);
    shadingModel.overrideSceneMaterials(collection.root, collection.comp);

    /*
    Render loop!
    */

    while (collection.running) {
        collection.render();
    }

    /*
    Free resources
    */

    return 0;
}
