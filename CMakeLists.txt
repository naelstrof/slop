#Change this if you need to target a specific CMake version
cmake_minimum_required(VERSION 3.1)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

if( NOT CMAKE_INSTALL_PREFIX )
    set( CMAKE_INSTALL_PREFIX "/usr" )
endif()

if( NOT SHADER_PREFIX )
    set( SHADER_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH "The path where slop thinks it resides in, so that it can find the shaders. This is not necessarily the CMAKE_INSTALL_PREFIX. For example if the shaders will exist in /usr/share/slop, SHADER_PREFIX should be \"/usr\"." )
endif()

project(slop)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wno-missing-braces")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

include_directories("${PROJECT_BINARY_DIR}")

# Define sources and executable
set(EXECUTABLE_NAME "slop")

set( source src/x.cpp
            src/window.cpp
            src/mouse.cpp
            src/keyboard.cpp
            src/framebuffer.cpp
            src/resource.cpp
            src/options.cpp
            src/gl_core_3_3.c
            src/shader.cpp
            src/slopstates.cpp
            src/slop.cpp
            src/rectangle.cpp
            src/main.cpp )

add_executable(${EXECUTABLE_NAME} ${source})


# Detect and add SFML
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules" )
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(OpenGL REQUIRED)
find_package(GLM REQUIRED)
find_package(GLX REQUIRED)
find_package(X11 REQUIRED)
find_package(XExt REQUIRED) 

include_directories(${X11_INCLUDE_DIR}
                    ${GLM_INCLUDE_DIR}
                    ${GLX_INCLUDE_DIR}
                    ${OPENGL_INCLUDE_DIR})

target_link_libraries(${EXECUTABLE_NAME} ${X11_LIBRARIES}
                                         ${GLX_LIBRARIES}
                                         ${OPENGL_LIBRARIES})

# Install targets
install( TARGETS ${EXECUTABLE_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )
install( DIRECTORY "${CMAKE_SOURCE_DIR}/share" DESTINATION "${CMAKE_INSTALL_PREFIX}" )

add_definitions(-DSHADER_PREFIX="${SHADER_PREFIX}")
