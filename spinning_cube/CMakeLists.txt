# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (Tutorials)
set(EXE_NAME spinning_cube)

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
add_subdirectory (../engine_template build)

# On Visual 2005 and above, this module can set the debug working directory
if(POLICY CMP0026)
   cmake_policy(SET CMP0026 OLD)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

include_directories(
	../external/glm-0.9.7.1/
	../external/glew-1.13.0/include/
	../external/bass24/
        ../external/ImGuiColorTextEdit/
        ../external/imgui/
        ../external/imgui/examples/
        ../fswatch-1.14.0/libfswatch/src/libfswatch/c++/
        ../external/stb/
	../engine_template
	.
)

set(ALL_LIBS
        gengine
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
        ${FSWATCH}
)

#set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

add_executable(${EXE_NAME}
	spinning_cube.cpp
        Cube.cpp
)
target_link_libraries(${EXE_NAME}
	${ALL_LIBS}
)
set_property(TARGET ${EXE_NAME} PROPERTY CXX_STANDARD 11)

# Xcode and Visual working directories
set_target_properties(${EXE_NAME} PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
create_target_launcher(${EXE_NAME} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")

##################################################################

SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )

if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET ${EXE_NAME} POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${EXE_NAME}${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/"
)

# Copy bass library
add_custom_command(
   TARGET ${EXE_NAME} POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/../external/bass24/libbass.dylib" "${CMAKE_CURRENT_SOURCE_DIR}/"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

