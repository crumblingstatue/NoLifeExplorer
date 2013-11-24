project(NoLifeExplorer)
cmake_minimum_required(VERSION 2.8.12)

# Detect compiler
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(USING_GCC 1)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(USING_CLANG 1)
endif()

# For GCC and Clang, enable C++11 support and add some other flags
if(USING_GCC OR USING_CLANG)
  add_compile_options(-std=c++11 -pedantic -Wall -Wextra -Wshadow)
endif()

# Disable Clang warnings about nested anon types
if(USING_CLANG)
  add_compile_options(-Wno-nested-anon-types)
endif()

add_subdirectory(NoLifeNx)

find_package(Qt5Widgets REQUIRED)
find_package(SFML 2 COMPONENTS audio system REQUIRED)
include_directories(${SFML_INCLUDE_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
qt5_wrap_ui(UIS_HDRS
mainwindow.ui
)

add_executable(nlexplorer
NodeItem.h
Sound.cpp
Sound.h
main.cpp
mainwindow.cpp
mainwindow.h
soundplayerwidget.cpp
soundplayerwidget.h
${UIS_HDRS}
)

qt5_use_modules(nlexplorer Widgets)

target_link_libraries(nlexplorer NoLifeNx ${SFML_LIBRARIES} lz4 mpg123)