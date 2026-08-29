cmake_minimum_required(VERSION 3.20)
project(netsim LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Packet library
add_library(packet STATIC IMPORTED)
set_target_properties(packet PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/packet.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/lib"
)


# Netsim library
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB_RECURSE sources src/*.cpp)
add_library(netsim STATIC ${sources})
target_link_libraries(netsim PRIVATE packet)
target_include_directories(netsim PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Netsim test
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE gui_sources gui/*.cpp)

add_executable(netsim_gui
    ${gui_sources}
    gui/gui.ui
)
target_include_directories(netsim_gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/gui)
target_link_libraries(netsim_gui PRIVATE
    netsim
    Qt6::Widgets
    Qt6::Gui
)
