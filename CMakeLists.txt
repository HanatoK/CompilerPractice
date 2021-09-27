cmake_minimum_required(VERSION 3.14)

project(CompilerPractice LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(PROJECT_SOURCE_DIR src)

#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(QT_VERSION_MAJOR 5)
#add_compile_definitions(DEBUG_DESTRUCTOR)

include_directories (
  ${PROJECT_SOURCE_DIR}/Parsers
  ${PROJECT_SOURCE_DIR}/Executors
  ${PROJECT_SOURCE_DIR}
)

file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h ${PROJECT_SOURCE_DIR}/Parsers/*.h ${PROJECT_SOURCE_DIR}/Executors/*.h)
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/Parsers/*.cpp ${PROJECT_SOURCE_DIR}/Executors/*.cpp)

#find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

add_executable(CompilerPractice ${HEADERS} ${SOURCES})
#target_link_libraries(CompilerPractice Qt${QT_VERSION_MAJOR}::Core fmt)
target_link_libraries(CompilerPractice fmt)
