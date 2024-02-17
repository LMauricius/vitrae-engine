cmake_minimum_required(VERSION 3.7.0)

file(GLOB_RECURSE SrcFiles CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HeaderFiles CONFIGURE_DEPENDS include/*.h)

add_executable(VitraeShowcase ${SrcFiles} ${HeaderFiles})
target_include_directories(VitraeShowcase PUBLIC include)
target_include_directories(VitraeEngine PUBLIC dependencies/)
target_include_directories(VitraeEngine PUBLIC dependencies/glad/include)
target_include_directories(VitraeEngine PUBLIC dependencies/DynAsMa/include)