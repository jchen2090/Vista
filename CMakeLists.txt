cmake_minimum_required(VERSION 3.10)
project(Vista CXX)

# Set C++ standard (good practice, replacing standard flags)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compile flags (equivalent to your CXXFLAGS)
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -g)
endif()

# Explicitly list your source files
set(SRCS 
    src/lexer/lexer.cpp 
    src/token/token.cpp
)

set (TEST_SRCS
    tests/lexer.cpp 
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define the final executable output and target name
add_executable(compiler src/main.cpp ${SRCS})

# Set the output directory to match your old build/ directory layout
set_target_properties(compiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    OUTPUT_NAME "compiler"
)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests "${TEST_SRCS}" "${SRCS}")
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
