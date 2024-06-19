cmake_minimum_required(VERSION 3.7.0)

find_package(Qt5Widgets)

file(GLOB_RECURSE SrcFiles CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HeaderFiles CONFIGURE_DEPENDS include/*.h)
file(GLOB_RECURSE FORMS CONFIGURE_DEPENDS forms/**.ui)

qt5_wrap_cpp(MOC_SOURCES ${SOURCES} ${HEADERS} TARGET VitraeShowcase)
qt5_wrap_ui(UI_SOURCES ${FORMS})
add_executable(VitraeShowcase ${SrcFiles} ${HeaderFiles} ${MOC_SOURCES} ${UI_SOURCES})
target_link_libraries(VitraeShowcase PRIVATE VitraeEngine Qt5::Core Qt5::Gui Qt5::Widgets)
target_include_directories(VitraeShowcase PUBLIC include)
target_include_directories(VitraeEngine PUBLIC dependencies/)
target_include_directories(VitraeEngine PUBLIC dependencies/glad/include)
target_include_directories(VitraeEngine PUBLIC dependencies/DynAsMa/include)