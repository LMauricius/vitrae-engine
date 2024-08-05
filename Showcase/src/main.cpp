#include <QtWidgets/QApplication>
#include <iostream>
#include <thread>

#include "SettingsWindow.h"
#include "Status.hpp"
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
    Status status;

    /*
    GUI setup
    */
    QApplication app(argc, argv);
    SettingsWindow settingsWindow(collection, status);
    settingsWindow.show();

    /*
    Render loop!
    */
    while (collection.running) {
        app.processEvents();

        {
            std::unique_lock lock1(collection.accessMutex);

            auto startTime = std::chrono::high_resolution_clock::now();
            collection.render();
            auto endTime = std::chrono::high_resolution_clock::now();

            status.update(endTime - startTime);
        }

        settingsWindow.updateValues();
    }

    /*
    Free resources
    */
    p_rend->free();

    return 0;
}
