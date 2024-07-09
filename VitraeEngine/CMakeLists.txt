cmake_minimum_required(VERSION 3.7.0)

set(VITRAE_ENABLE_STRINGID_DEBUGGING OFF CACHE BOOL "Whether StringId objects will keep info on original strings for debugging purposes. Has performance penalty.")

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

if(VITRAE_ENABLE_STRINGID_DEBUGGING)
    target_compile_definitions(VitraeEngine PUBLIC VITRAE_DEBUG_STRINGIDS)
endif()