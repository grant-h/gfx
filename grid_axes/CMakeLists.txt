# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (Tutorials)

find_package(OpenGL REQUIRED)


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

# Compile external dependencies 
add_subdirectory (../external build)

# On Visual 2005 and above, this module can set the debug working directory
if(POLICY CMP0026)
   cmake_policy(SET CMP0026 OLD)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

include_directories(
	../external/glfw-3.1.2/include/
	../external/glm-0.9.7.1/
	../external/glew-1.13.0/include/
	../
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

add_executable(engine_demo
	engine_demo.cpp
	../common/shader.cpp
	../common/shader.hpp
	../common/controls.cpp
	../common/controls.hpp
	../common/texture.cpp
	../common/texture.hpp
	../common/objloader.cpp
	../common/objloader.hpp
	../common/vboindexer.cpp
	../common/vboindexer.hpp
	../common/plyloader.cpp
	../common/plyloader.hpp
	../common/Object.h
	../common/util.h
	../common/tga.c
	../common/tga.h
)
target_link_libraries(engine_demo
	${ALL_LIBS}
)
set_property(TARGET engine_demo PROPERTY CXX_STANDARD 11)

# Xcode and Visual working directories
set_target_properties(engine_demo PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
create_target_launcher(engine_demo WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")

##################################################################

SOURCE_GROUP(common REGULAR_EXPRESSION "../common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )

if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET engine_demo POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/engine_demo${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/"
)
elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

