project(NoLifeExplorer)
cmake_minimum_required(VERSION 2.8.12)

set(NOLIFEEXPLORER_RELEASE 9)
set(NOLIFEEXPLORER_CODENAME "Rusted Edge")

configure_file(
  "${PROJECT_SOURCE_DIR}/config.hpp.in"
  "${PROJECT_BINARY_DIR}/config.hpp"
)

# Detect compiler
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(USING_GCC TRUE)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(USING_CLANG TRUE)
endif()

# For GCC and Clang, enable C++11 support and add some other flags
if(USING_GCC OR USING_CLANG)
  add_compile_options(-std=c++11 -pedantic -Wall -Wextra -Wconversion
                      -Wshadow -Wmissing-declarations -Wnon-virtual-dtor)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)
find_package(NoLifeNx REQUIRED)
include_directories(${NOLIFENX_INCLUDE_DIR})

find_package(Qt5Widgets REQUIRED)
find_package(SFML 2 COMPONENTS audio system REQUIRED)
include_directories(${SFML_INCLUDE_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(nlexplorer
src/AudioPlayerWidget.cpp
src/AudioPlayerWidget.hpp
src/AudioStream.cpp
src/AudioStream.hpp
src/MainWindow.cpp
src/MainWindow.hpp
src/NodeItem.cpp
src/NodeItem.hpp
src/main.cpp
src/nodeUtil.cpp
src/nodeUtil.hpp
)

qt5_use_modules(nlexplorer Widgets)

target_link_libraries(nlexplorer ${NOLIFENX_LIBRARY} ${SFML_LIBRARIES} lz4 mpg123)
