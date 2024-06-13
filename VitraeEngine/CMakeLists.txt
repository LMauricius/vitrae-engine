cmake_minimum_required(VERSION 3.7.0)

file(GLOB_RECURSE SrcFiles CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HeaderFiles CONFIGURE_DEPENDS include/*.h include/*.hpp)
set(GLAD_SrcFile ../VitraeEngine/dependencies/glad/src/glad.c)

add_library(VitraeEngine ${SrcFiles} ${HeaderFiles} ${GLAD_SrcFile})
target_compile_features(VitraeEngine PUBLIC cxx_std_20)
target_include_directories(VitraeEngine PUBLIC include)
target_include_directories(VitraeEngine PUBLIC ../VitraeEngine/dependencies/)
target_include_directories(VitraeEngine PUBLIC ../VitraeEngine/dependencies/glad/include)
target_include_directories(VitraeEngine PUBLIC ../VitraeEngine/dependencies/DynAsMa/include)
target_link_libraries(VitraeEngine PUBLIC assimp glfw)