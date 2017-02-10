cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)")
if ( NOT CMAKE_INSTALL_PREFIX )
    set(CMAKE_INSTALL_PREFIX "/usr")
endif()

if ( NOT SLOP_LEGACY_MODE )
    set(SLOP_LEGACY_MODE FALSE CACHE BOOL "Legacy mode requires the XShape extension, is laggy, isn't guaranteed to be invisible on shutdown, isn't guaranteed to support opacity, doesn't support custom shaders, and requires C++11 for the sleep_for functions. Though it still might be desired since it doesn't require OpenGL to use. Your choice <:o)")
endif()

project(slop)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

include_directories("${PROJECT_BINARY_DIR}")

add_definitions(-DSLOP_VERSION="v5.3.21")

# The names have to be unique unfortunately.
set(EXECUTABLE_NAME "slop")
set(LIBRARY_NAME "slopy")
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules" )

if ( SLOP_LEGACY_MODE )
    add_definitions(-DSLOP_LEGACY_MODE=true)
    add_library(${LIBRARY_NAME} SHARED  src/mouse.cpp
                                        src/keyboard.cpp
                                        src/x.cpp
                                        src/slopstates.cpp
                                        src/windowhelper.cpp
                                        src/resource.cpp
                                        src/xslop.cpp
                                        src/xshaperectangle.cpp)
    set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD 11)
    set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

    add_executable(${EXECUTABLE_NAME} src/options.cpp
                                      src/main.cpp)

    # Detect and add cmake modules
    find_package(GLM REQUIRED)
    find_package(X11 REQUIRED)
    find_package(XExt REQUIRED)
    include_directories(${X11_INCLUDE_DIR}
                        ${GLM_INCLUDE_DIR}
                        ${XEXT_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${X11_LIBRARIES}
                                             ${GLM_LIBRARIES}
                                             ${XEXT_LIBRARIES}
                                             ${LIBRARY_NAME})
    # Install targets
    install( TARGETS ${EXECUTABLE_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )
    install( TARGETS ${LIBRARY_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" )
    install( FILES ${CMAKE_SOURCE_DIR}/src/slop.hpp DESTINATION "${CMAKE_INSTALL_PREFIX}/include" )
else()
    add_library(${LIBRARY_NAME} SHARED  src/mouse.cpp
                                        src/keyboard.cpp
                                        src/x.cpp
                                        src/slopstates.cpp
                                        src/framebuffer.cpp
                                        src/windowhelper.cpp
                                        src/resource.cpp
                                        src/gl_core_3_3.c
                                        src/shader.cpp
                                        src/window.cpp
                                        src/glslop.cpp
                                        src/rectangle.cpp)

    add_executable(${EXECUTABLE_NAME} src/options.cpp
                                      src/main.cpp)

    # Detect and add cmake modules
    find_package(GLM REQUIRED)
    find_package(X11 REQUIRED)
    find_package(GLX REQUIRED)
    find_package(OpenGL REQUIRED)
    include_directories(${X11_INCLUDE_DIR}
                        ${GLM_INCLUDE_DIR}
                        ${GLX_INCLUDE_DIR}
                        ${OPENGL_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${X11_LIBRARIES}
                                             ${GLM_LIBRARIES}
                                             ${OPENGL_LIBRARIES}
                                             ${GLX_LIBRARY}
                                             ${LIBRARY_NAME})
    # Install targets
    install( TARGETS ${EXECUTABLE_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )
    install( TARGETS ${LIBRARY_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" )
    install( FILES ${CMAKE_SOURCE_DIR}/src/slop.hpp DESTINATION "${CMAKE_INSTALL_PREFIX}/include" )
endif()
