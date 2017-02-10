cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)")
if ( NOT CMAKE_INSTALL_PREFIX )
    set(CMAKE_INSTALL_PREFIX "/usr")
endif()

if ( NOT SLOP_LEGACY_MODE )
    set(SLOP_LEGACY_MODE FALSE CACHE BOOL "Legacy mode requires the XShape extension, is laggy, isn't guaranteed to be invisible on shutdown, isn't guaranteed to support opacity, doesn't support custom shaders, and requires C++11 for the sleep_for functions. Though it still might be desired since it doesn't require OpenGL to use.")
endif()

project(slop)

add_definitions(-DSLOP_VERSION="v5.3.21")
if ( SLOP_LEGACY_MODE )
    add_definitions(-DSLOP_LEGACY_MODE=true)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

include_directories("${PROJECT_BINARY_DIR}")

# Define sources and executable
set(EXECUTABLE_NAME "slop")

set( source src/x.cpp
            src/windowhelper.cpp
            src/mouse.cpp
            src/keyboard.cpp
            src/resource.cpp
            src/options.cpp
            src/slopstates.cpp
            src/main.cpp )

if ( SLOP_LEGACY_MODE )
    set ( source ${source}
                 src/xslop.cpp
                 src/xshaperectangle.cpp )
else()
    set ( source ${source}
                 src/window.cpp
                 src/glslop.cpp
                 src/rectangle.cpp
                 src/framebuffer.cpp
                 src/gl_core_3_3.c
                 src/shader.cpp )
endif()

add_executable(${EXECUTABLE_NAME} ${source})

# Detect and add cmake modules
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules" )
find_package(GLM REQUIRED)
find_package(X11 REQUIRED)

include_directories(${X11_INCLUDE_DIR}
                    ${GLM_INCLUDE_DIR})
target_link_libraries(${EXECUTABLE_NAME} ${X11_LIBRARIES}
                                         ${GLM_LIBRARIES})

# Either we require OpenGL, or we require the XShape extension.
if ( SLOP_LEGACY_MODE )
    find_package(XExt REQUIRED)
    include_directories(${XEXT_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${XEXT_LIBRARIES})
    set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD 11)
    set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
else()
    find_package(GLX REQUIRED)
    find_package(OpenGL REQUIRED)
    include_directories(${GLX_INCLUDE_DIR}
                        ${OPENGL_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${OPENGL_LIBRARIES}
                                             ${GLX_LIBRARY})
endif()

# Install targets
install( TARGETS ${EXECUTABLE_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )
