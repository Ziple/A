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

# --------------------------------------------
# Add the folder to the compilation includes path
# --------------------------------------------
include_directories( include )

file(
    GLOB_RECURSE
    proc_source_files
    src/*.cpp
)

file(
    GLOB_RECURSE
    proc_header_files
    include/*.hpp
)

add_executable(
    A
    ${proc_source_files}
    ${proc_header_files}
)

target_link_libraries( A SDL2 );

find_package( OpenGL )
if( OPENGL_FOUND )

	include_directories( ${OPENGL_INCLUDE_DIR} )
	target_link_libraries( A ${OPENGL_gl_LIBRARY} )

	if( CMAKE_SYSTEM_NAME STREQUAL "Linux" )
		target_link_libraries( A GLEW )
	elseif( CMAKE_SYSTEM_NAME STREQUAL "Windows" )
		target_link_libraries( A glew32 )
		target_link_libraries( A SDL2main )
	endif( CMAKE_SYSTEM_NAME STREQUAL "Linux" )

endif( OPENGL_FOUND )

