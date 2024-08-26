# Setup
cmake_minimum_required (VERSION 3.20)
project (zerotracer)
set (CMAKE_CXX_STANDARD 17)

# Find SDL2
find_package(SDL2 REQUIRED COMPONENTS SDL2)

# Add .cpp files into executable
file (GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/main.cpp")

add_executable (rt-weekend ${SRC_FILES})

# Create pre-compiled header
file (GLOB_RECURSE UTIL_HEADERS "${CMAKE_SOURCE_DIR}*.h")
target_precompile_headers (rt-weekend PRIVATE ${UTIL_HEADERS})

# Link SDL2
target_link_libraries(rt-weekend PRIVATE SDL2::SDL2)
