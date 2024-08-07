#pragma once

#include <iostream>

#include "Methods/abstract.hpp"
#include "shadingModes.hpp"

#include "Vitrae/ComponentRoot.hpp"
#include "Vitrae/Pipelines/Compositing/Task.hpp"
#include "Vitrae/Pipelines/Shading/Task.hpp"
#include "Vitrae/Visuals/Compositor.hpp"

#include <filesystem>
#include <mutex>

using namespace Vitrae;

struct MethodCategory
{
    std::string name;
    std::vector<std::shared_ptr<MethodCollection>> methods;
    std::size_t selectedIndex;
};

struct AssetCollection
{
    std::mutex accessMutex;

    bool running;
    bool shouldReloadPipelines;

    ComponentRoot &root;
    Renderer &rend;

    ShadingModeSetter modeSetter;
    std::vector<MethodCategory> methodCategories;

    dynasma::FirmPtr<FrameStore> p_windowFrame;
    dynasma::FirmPtr<Scene> p_scene;
    Compositor comp;

    AssetCollection(ComponentRoot &root, Renderer &rend, std::filesystem::path scenePath,
                    float sceneScale);
    ~AssetCollection();

    void reapplyChoosenMethods();
    void render();
};