cmake_minimum_required( VERSION 2.6 )

# --------------------------------------------
# Project name definition
# --------------------------------------------
project( "A" )

cmake_policy( SET CMP0015 NEW )

# --------------------------------------------
# Add the compiler include / lib paths
# to the default CMake paths
# --------------------------------------------
set(CMAKE_INCLUDE_PATH $CMAKE_INCLUDE_PATH $ENV{INCLUDE})
set(CMAKE_LIBRARY_PATH $CMAKE_LIBRARY_PATH $ENV{LIB})
set(CMAKE_LIBRARY_PATH $CMAKE_LIBRARY_PATH $LIBRARY_OUTPUT_PATH)

# --------------------------------------------
# Add the folder to the compilation includes path
# --------------------------------------------
include_directories( include )

file(
    GLOB_RECURSE
    a_source_files
    src/*.cpp
)

file(
    GLOB_RECURSE
    a_header_files
    include/*.hpp
)

add_executable(
    A
    ${a_source_files}
    ${a_header_files}
)

target_link_libraries( A SDL2 )
target_link_libraries( A GLEW )

find_package( OpenGL )
target_link_libraries( A ${OPENGL_gl_LIBRARY} )

