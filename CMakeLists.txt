cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)")
if ( NOT CMAKE_INSTALL_PREFIX )
    set(CMAKE_INSTALL_PREFIX "/usr")
endif()

project(slop)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

set( CMAKE_INSTALL_MANDIR "${CMAKE_INSTALL_PREFIX}/share/man" CACHE PATH "Directory where man pages reside. (/usr/share/man, /usr/local/share/man, etc.)" )

set( CMAKE_COMPRESS_MAN TRUE CACHE BOOL "Whether or not to compress the man pages for install." )

if ( CMAKE_COMPRESS_MAN )
    set( MANTARGET "slop.1.gz" )
else()
    set( MANTARGET "slop.1" )
endif()

include_directories("${PROJECT_BINARY_DIR}")

add_definitions(-DSLOP_VERSION="v5.3.27")

# The names have to be unique unfortunately.
set(EXECUTABLE_NAME "slop")
set(LIBRARY_NAME "slopy")

add_library(${LIBRARY_NAME} SHARED  src/mouse.cpp
                                    src/keyboard.cpp
                                    src/x.cpp
                                    src/slopstates.cpp
                                    src/framebuffer.cpp
                                    src/resource.cpp
                                    src/gl_core_3_3.c
                                    src/shader.cpp
                                    src/window.cpp
                                    src/slop.cpp
                                    src/rectangle.cpp
                                    src/xshaperectangle.cpp
                                    src/glrectangle.cpp)

set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

add_executable(${EXECUTABLE_NAME} src/options.cpp
                                  src/main.cpp)

set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD 11)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules" )

find_package(GLM REQUIRED)
find_package(X11 REQUIRED)
find_package(XExt REQUIRED)
find_package(GLX REQUIRED)
find_package(OpenGL REQUIRED)
find_package(XRender REQUIRED)

include_directories(${X11_INCLUDE_DIR}
                    ${GLM_INCLUDE_DIR}
                    ${XEXT_INCLUDE_DIR}
                    ${GLX_INCLUDE_DIR}
                    ${XRENDER_INCLUDE_DIR}
                    ${OPENGL_INCLUDE_DIR})

target_link_libraries(${LIBRARY_NAME} ${X11_LIBRARIES}
                                      ${GLM_LIBRARIES}
                                      ${OPENGL_LIBRARIES}
                                      ${GLX_LIBRARY}
                                      ${XRENDER_LIBRARY}
                                      ${XEXT_LIBRARIES})

target_link_libraries(${EXECUTABLE_NAME} ${LIBRARY_NAME})

install( TARGETS ${EXECUTABLE_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )
install( TARGETS ${LIBRARY_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" )
install( FILES ${CMAKE_SOURCE_DIR}/src/slop.hpp DESTINATION "${CMAKE_INSTALL_PREFIX}/include" COMPONENT development )
install( FILES "${CMAKE_SOURCE_DIR}/${MANTARGET}" DESTINATION  "${CMAKE_INSTALL_MANDIR}/man1" COMPONENT doc )
