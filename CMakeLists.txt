cmake_minimum_required(VERSION 3.16)
project(TextEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/editor.cpp
    src/documentmanager.cpp
    src/undoredostack.cpp
    src/searchreplace.cpp
    src/syntaxhighlighter.cpp
    include/mainwindow.h
    include/editor.h
    include/documentmanager.h
    include/undoredostack.h
    include/searchreplace.h
    include/syntaxhighlighter.h
    ui/mainwindow.ui
    resources/resources.qrc
)

add_executable(TextEditor ${PROJECT_SOURCES})

target_include_directories(TextEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(TextEditor
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Set output directory
set_target_properties(TextEditor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
