cmake_minimum_required(VERSION 3.7.0)

file(GLOB_RECURSE SrcFiles CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HeaderFiles CONFIGURE_DEPENDS include/*.h)

add_library(VitraeEngine ${SrcFiles} ${HeaderFiles})
target_compile_features(VitraeEngine PUBLIC cxx_std_20)
target_include_directories(VitraeEngine PUBLIC include)
target_include_directories(VitraeEngine PUBLIC dependencies/)
target_include_directories(VitraeEngine PUBLIC dependencies/glad/include)
target_link_libraries(VitraeEngine assimp glfw)