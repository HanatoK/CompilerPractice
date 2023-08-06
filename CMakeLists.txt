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

# avoid using GLOB (see https://cmake.org/cmake/help/latest/command/file.html?highlight=glob#file)
#file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h ${PROJECT_SOURCE_DIR}/Parsers/*.h ${PROJECT_SOURCE_DIR}/Executors/*.h)
#file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/Parsers/*.cpp ${PROJECT_SOURCE_DIR}/Executors/*.cpp)

set(HEADERS ${PROJECT_SOURCE_DIR}/Backend.h
            ${PROJECT_SOURCE_DIR}/Common.h
            ${PROJECT_SOURCE_DIR}/Compiler.h
            ${PROJECT_SOURCE_DIR}/Frontend.h
            ${PROJECT_SOURCE_DIR}/Intermediate.h
            ${PROJECT_SOURCE_DIR}/IntermediateImpl.h
            ${PROJECT_SOURCE_DIR}/Interpreter.h
            ${PROJECT_SOURCE_DIR}/Pascal.h
            ${PROJECT_SOURCE_DIR}/PascalFrontend.h
            ${PROJECT_SOURCE_DIR}/Predefined.h
            ${PROJECT_SOURCE_DIR}/TypeChecker.h
            ${PROJECT_SOURCE_DIR}/Utilities.h
            ${PROJECT_SOURCE_DIR}/Parsers/ArrayTypeParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/AssignmentStatementParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/BlockParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/CaseStatementParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/CompoundStatementParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/ConstantDefinitionsParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/DeclarationsParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/EnumerationTypeParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/ExpressionParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/ForStatementParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/IfStatementParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/ProgramParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/RecordTypeParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/RepeatStatementParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/SimpleTypeParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/StatementParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/SubrangeTypeParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/TypeDefinitionsParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/TypeSpecificationParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/WhileStatementParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/VariableDeclarationsParser.h
            ${PROJECT_SOURCE_DIR}/Executors/AssignmentStatementExecutor.h
            ${PROJECT_SOURCE_DIR}/Executors/CompoundStatementExecutor.h
            ${PROJECT_SOURCE_DIR}/Executors/ExpressionExecutor.h
            ${PROJECT_SOURCE_DIR}/Executors/IfExecutor.h
            ${PROJECT_SOURCE_DIR}/Executors/LoopExecutor.h
            ${PROJECT_SOURCE_DIR}/Executors/SelectExecutor.h
            ${PROJECT_SOURCE_DIR}/Executors/StatementExecutor.h
            ${PROJECT_SOURCE_DIR}/Parsers/VariableParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/DeclaredRoutineParser.h
            ${PROJECT_SOURCE_DIR}/Parsers/CallParser.h)

set(SOURCES ${PROJECT_SOURCE_DIR}/main.cpp
            ${PROJECT_SOURCE_DIR}/Backend.cpp
            ${PROJECT_SOURCE_DIR}/Common.cpp
            ${PROJECT_SOURCE_DIR}/Compiler.cpp
            ${PROJECT_SOURCE_DIR}/Frontend.cpp
            ${PROJECT_SOURCE_DIR}/Intermediate.cpp
            ${PROJECT_SOURCE_DIR}/IntermediateImpl.cpp
            ${PROJECT_SOURCE_DIR}/Interpreter.cpp
            ${PROJECT_SOURCE_DIR}/Pascal.cpp
            ${PROJECT_SOURCE_DIR}/PascalFrontend.cpp
            ${PROJECT_SOURCE_DIR}/Predefined.cpp
            ${PROJECT_SOURCE_DIR}/TypeChecker.cpp
            ${PROJECT_SOURCE_DIR}/Utilities.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/ArrayTypeParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/AssignmentStatementParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/BlockParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/CaseStatementParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/CompoundStatementParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/ConstantDefinitionsParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/DeclarationsParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/EnumerationTypeParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/ExpressionParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/ForStatementParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/IfStatementParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/ProgramParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/RecordTypeParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/RepeatStatementParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/SimpleTypeParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/StatementParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/SubrangeTypeParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/TypeDefinitionsParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/TypeSpecificationParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/WhileStatementParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/VariableDeclarationsParser.cpp
            ${PROJECT_SOURCE_DIR}/Executors/AssignmentStatementExecutor.cpp
            ${PROJECT_SOURCE_DIR}/Executors/CompoundStatementExecutor.cpp
            ${PROJECT_SOURCE_DIR}/Executors/ExpressionExecutor.cpp
            ${PROJECT_SOURCE_DIR}/Executors/IfExecutor.cpp
            ${PROJECT_SOURCE_DIR}/Executors/LoopExecutor.cpp
            ${PROJECT_SOURCE_DIR}/Executors/SelectExecutor.cpp
            ${PROJECT_SOURCE_DIR}/Executors/StatementExecutor.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/VariableParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/DeclaredRoutineParser.cpp
            ${PROJECT_SOURCE_DIR}/Parsers/CallParser.cpp)

#find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

add_executable(CompilerPractice ${HEADERS} ${SOURCES})
#target_link_libraries(CompilerPractice Qt${QT_VERSION_MAJOR}::Core fmt)
target_link_libraries(CompilerPractice fmt)
