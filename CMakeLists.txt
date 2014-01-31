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

file(
	GLOB_RECURSE
	proc_shaders_files
	shaders/*.glsl
)
	
source_group( "Shaders" FILES ${proc_shaders_files} )

add_executable(
    A
    ${proc_source_files}
    ${proc_header_files}
	${proc_shaders_files}
)

# --------------------------------------------
# Add -d to debug lib names
# --------------------------------------------
set( kt_lib_debug_suffix "-d" )

# --------------------------------------------
# Add -s to lib names
# --------------------------------------------
if( NOT USE_SHARED_LIBS )
    set( kt_lib_suffix -s )
endif( NOT USE_SHARED_LIBS )

set( ktCoreLibName ktCore${kt_lib_suffix} )
set( ktWindowLibName ktWindow${kt_lib_suffix} )
set( ktGraphicsLibName ktGraphics${kt_lib_suffix} )
set( ktOGL3DeviceLibName ktOGL3Device${kt_lib_suffix} )
set( ktGameLibName ktGame${kt_lib_suffix} )

target_link_libraries( A optimized ${ktCoreLibName} )
target_link_libraries( A optimized ${ktWindowLibName} )
target_link_libraries( A optimized ${ktGraphicsLibName} )
target_link_libraries( A optimized ${ktOGL3DeviceLibName} )
target_link_libraries( A optimized ${ktGameLibName} )
target_link_libraries( A optimized ktMain )

target_link_libraries( A debug ${ktCoreLibName}${kt_lib_debug_suffix} )
target_link_libraries( A debug ${ktWindowLibName}${kt_lib_debug_suffix} )
target_link_libraries( A debug ${ktGraphicsLibName}${kt_lib_debug_suffix} )
target_link_libraries( A debug ${ktOGL3DeviceLibName}${kt_lib_debug_suffix} )
target_link_libraries( A debug ${ktGameLibName}${kt_lib_debug_suffix} )
target_link_libraries( A debug ktMain${kt_lib_debug_suffix} )

target_link_libraries( A SDL2 )

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

