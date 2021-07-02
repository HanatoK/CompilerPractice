cmake_minimum_required(VERSION 3.14)

project(CompilerPractice LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEADERS
    Frontend.h
    Intermediate.h
    Backend.h
    Pascal.h)

set(SOURCES
    main.cpp
    Frontend.cpp
    Backend.cpp
    Pascal.cpp)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

add_executable(CompilerPractice ${SOURCES} ${HEADERS})
target_link_libraries(CompilerPractice Qt${QT_VERSION_MAJOR}::Core)
