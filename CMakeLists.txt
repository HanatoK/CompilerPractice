cmake_minimum_required(VERSION 3.14)

project(CompilerPractice LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_VERSION_MAJOR 5)
add_compile_definitions(DEBUG_DESTRUCTOR)

set(HEADERS
    Frontend.h
    Intermediate.h
    IntermediateImpl.h
    Backend.h
    Pascal.h
    PascalFrontend.h
    Common.h
    Utilities.h
    StatementParser.h
    CompoundStatementParser.h
    AssignmentStatementParser.h
    ExpressionParser.h)

set(SOURCES
    main.cpp
    Frontend.cpp
    Backend.cpp
    Pascal.cpp
    PascalFrontend.cpp
    Intermediate.cpp
    IntermediateImpl.cpp
    Utilities.cpp
    StatementParser.cpp
    CompoundStatementParser.cpp
    AssignmentStatementParser.cpp
    ExpressionParser.cpp)

#find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

add_executable(CompilerPractice ${SOURCES} ${HEADERS})
target_link_libraries(CompilerPractice Qt${QT_VERSION_MAJOR}::Core fmt)
