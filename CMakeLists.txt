cmake_minimum_required(VERSION 3.14)
project(Vista C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find LLVM from system 
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Map required LLVM components to library targets
llvm_map_components_to_libnames(llvm_libs core support irreader analysis)

# Compile flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -g)
endif()

# Explicitly list source files
set(SRCS 
    src/lexer/lexer.cpp 
    src/token/token.cpp
    src/ast/ast.cpp
    src/parser/parser.cpp
    src/type/typeChecker.cpp
    src/codegen/codegen.cpp
)

# TODO: Fix typeChecker tests
set(TEST_SRCS
    tests/lexer.cpp 
    tests/parser.cpp
    tests/typeChecker.cpp
)

# ------------------------------------------------------------------------------
# Executable Target: compiler
# ------------------------------------------------------------------------------
add_executable(compiler src/main.cpp ${SRCS})

set_target_properties(compiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    OUTPUT_NAME "compiler"
)

target_include_directories(compiler PRIVATE ${LLVM_INCLUDE_DIRS} src)
target_compile_definitions(compiler PRIVATE ${LLVM_DEFINITIONS})
target_link_libraries(compiler PRIVATE ${llvm_libs})

# ------------------------------------------------------------------------------
# Test Target: Catch2 & Unit Tests
# ------------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests ${TEST_SRCS} ${SRCS})

target_include_directories(tests PRIVATE ${LLVM_INCLUDE_DIRS} src)
target_compile_definitions(tests PRIVATE ${LLVM_DEFINITIONS})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain ${llvm_libs})
