cmake_minimum_required(VERSION 3.1.3)

project(slop VERSION 7.3.49 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "..." FORCE)
endif()

include(GNUInstallDirs)

# This should really never be disabled. The pure-X mode of slop is very expensive and buggy.
# It also doesn't work on Wayland. Though if a system is never running a compositor, or
# doesn't have OpenGL, this could remove some linking dependencies I suppose.
set( SLOP_OPENGL TRUE CACHE BOOL "To enable or disable OpenGL support." )
if ( SLOP_OPENGL )
  add_definitions(-DSLOP_OPENGL="True")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

include_directories("${PROJECT_BINARY_DIR}")

add_definitions(-DSLOP_VERSION="v${PROJECT_VERSION}")

# The names have to be unique unfortunately.
set(EXECUTABLE_NAME "slop")
set(LIBRARY_NAME "slopy")


if ( SLOP_OPENGL )
  add_library(${LIBRARY_NAME} SHARED  src/mouse.cpp
                                      src/keyboard.cpp
                                      src/x.cpp
                                      src/slopstates.cpp
                                      src/resource.cpp
                                      src/slop.cpp
                                      src/rectangle.cpp
                                      src/xshaperectangle.cpp
                                      src/shader.cpp
                                      src/window.cpp
                                      src/framebuffer.cpp
                                      src/glrectangle.cpp)
else()
  add_library(${LIBRARY_NAME} SHARED  src/mouse.cpp
                                      src/keyboard.cpp
                                      src/x.cpp
                                      src/slopstates.cpp
                                      src/resource.cpp
                                      src/slop.cpp
                                      src/rectangle.cpp
                                      src/xshaperectangle.cpp)
endif()

set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_EXTENSIONS OFF)

add_executable(${EXECUTABLE_NAME} src/main.cpp)

set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_EXTENSIONS OFF)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules" )

find_package(GLM REQUIRED)
find_package(X11 REQUIRED)
find_package(XExt REQUIRED)

if ( SLOP_OPENGL )
  find_package(GLEW REQUIRED)
  find_package(OpenGL REQUIRED)
  find_package(GLX REQUIRED)
  find_package(XRender REQUIRED)
  # Ahhhh, finally this eliminates the segfault on preload.
  find_package(Threads REQUIRED)
endif()

include_directories(${X11_INCLUDE_DIR}
                    ${GLM_INCLUDE_DIR}
                    ${GLX_INCLUDE_DIR})

target_link_libraries(${LIBRARY_NAME} ${X11_LIBRARIES}
                                      ${GLM_LIBRARIES}
                                      ${XEXT_LIBRARIES})

if ( SLOP_OPENGL )
  include_directories( ${XEXT_INCLUDE_DIR}
                       ${GLEW_INCLUDE_DIR}
                       ${XRENDER_INCLUDE_DIR}
                       ${OPENGL_INCLUDE_DIR} )
  target_link_libraries(${LIBRARY_NAME} ${OPENGL_LIBRARIES}
                                        ${XRENDER_LIBRARY}
                                        ${CMAKE_THREAD_LIBS_INIT} 
                                        ${GLX_LIBRARY}
                                        ${GLEW_LIBRARIES})
endif()

target_link_libraries(${EXECUTABLE_NAME} ${LIBRARY_NAME} )

if( CMAKE<3.7 )
  message( WARNING "CMake version is below 3.7, CMake version >= 3.7 is required for unicode support." )
else()
  find_package(ICU COMPONENTS uc)
  set( SLOP_UNICODE TRUE CACHE BOOL "To enable or disable unicode support." )
  if ( SLOP_UNICODE AND ICU_FOUND )
    add_definitions(-DCXXOPTS_USE_UNICODE)
    include_directories( ${ICU_INCLUDE_DIR} )
    target_link_libraries(${EXECUTABLE_NAME} ${ICU_UC_LIBRARIES} )
  endif()
endif()

install( TARGETS ${EXECUTABLE_NAME} DESTINATION "${CMAKE_INSTALL_BINDIR}" )
install( TARGETS ${LIBRARY_NAME} DESTINATION "${CMAKE_INSTALL_LIBDIR}" )
install( FILES ${CMAKE_SOURCE_DIR}/src/slop.hpp DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT development )
install( FILES ${CMAKE_SOURCE_DIR}/slop.1 DESTINATION "${CMAKE_INSTALL_MANDIR}/man1" COMPONENT doc )
