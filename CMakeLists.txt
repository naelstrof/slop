cmake_minimum_required( VERSION 2.8 )

project( "slop" )
set( slop_VERSION_MAJOR 4 )
set( slop_VERSION_MINOR 3 )
set( slop_VERSION_PATCH 20 )

set( CMAKE_OPENGL_SUPPORT FALSE CACHE BOOL "Whether or not to compile with OpenGL, shaders, magnification, and theming support." )

set( BIN_TARGET     "${PROJECT_NAME}" )
if( NOT CMAKE_INSTALL_PREFIX )
   set( CMAKE_INSTALL_PREFIX "/usr" )
endif()

if( NOT INSTALL_PREFIX )
   set( INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH "The path where slop thinks it resides in, so that it can find the shaders. This is not necessarily the CMAKE_INSTALL_PREFIX. For example if the shaders will exist in /usr/share/slop, INSTALL_PREFIX should be \"/usr\"." )
endif()

if( NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE )
   set( CMAKE_BUILD_TYPE RelWithDebInfo )
endif()

# Linux compiler initialization.
if ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
     "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
     "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" )
     set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-parameter" )
     set( CMAKE_CXX_FLAGS_DEBUG "-Wextra -pedantic-errors -O0 -g" )
     set( CMAKE_CXX_FLAGS_RELEASE "-O2" )
     set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g" )
         # -Wall:   Enable all warnings.
         # -Wextra: Enable some more warnings.
         # -Werror: Have errors on warnings.
         # -pedantic-errors: Even more errors.
         # -Wno-unused-parameter: Prevent unused variable warning. (Several functions are required to have unecessary variables because X11.)
else()
    message( FATAL_ERROR "Your operating system isn't supported yet! CMake will now exit." )
endif()

# Add a check target for our makefile.
find_program( CPPCHECK_EXECUTABLE cppcheck
              DOC "A tool for static C/C++ code analysis." )
if (CPPCHECK_EXECUTABLE)
    add_custom_target( "check"
                       COMMAND "${CPPCHECK_EXECUTABLE}" "--enable=all" "*"
                       WORKING_DIRECTORY src VERBATIM )
endif()

# Here we generate some of our code if we can. I package it pre-generated
# so nobody has to go find and install gengetopt if they don't want to.
find_program( GENGETOPT_EXECUTABLE gengetopt
              DOC "A tool to generate code to grab command line options." )
find_program( SED_EXECUTABLE sed )
if ( GENGETOPT_EXECUTABLE AND SED_EXECUTABLE )
    message( "-- Regenerating cmdline.in" )
    # gengetopt generates cmdline.h, then we move it to cmdline.in.
    execute_process( COMMAND "${GENGETOPT_EXECUTABLE}" "--input=options.ggo"
                     WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src" )
    file( RENAME "${CMAKE_CURRENT_SOURCE_DIR}/src/cmdline.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/cmdline.in" )
    # Due to a bug in gengetopt, we have to manually insert some code.
    # Replace the first instance of REPLACEME with some text.
    # Eight backslashes = two in the code because of how many instances of escaping is happening.
    execute_process( COMMAND "${SED_EXECUTABLE}" "-i" "0,/REPLACEME/{s/REPLACEME/X=%x\\\\\\\\nY=%y\\\\\\\\nW=%w\\\\\\\\nH=%h\\\\\\\\nG=%g\\\\\\\\nID=%i\\\\\\\\nCancel=%c\\\\\\\\n/}" "cmdline.c"
                     WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src" )
    # Then replace remaining instances.
    execute_process( COMMAND "${SED_EXECUTABLE}" "-i" "s/REPLACEME/X=%x\\\\nY=%y\\\\nW=%w\\\\nH=%h\\\\nG=%g\\\\nID=%i\\\\nCancel=%c\\\\n/" "cmdline.c"
                     WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src" )
else()
    message( "Warning: Command gengetopt or sed not found! Won't regenerate command line code. (If you're just compiling this doesn't matter.)" )
endif()

# By default our src/options.ggo has our cmake versions variables for
# the 'version ""' line. We replace them here.
# The ${CMAKE_SOURCE_DIR} is there to fix problems with OpenBSD's out-of-source build black magic.
configure_file( "src/cmdline.in" "${CMAKE_SOURCE_DIR}/src/cmdline.h" )

# This allows for "make README.md" to be ran to update the README's help
# section automatically. We don't add it to ALL because running arbitrary
# scripts is unsafe and I don't know if systems will actually have it
# be executbable.
add_custom_target( README.md "./generateReadme.sh" DEPENDS "slop" )

# Sources
if( CMAKE_OPENGL_SUPPORT )
    set( source
         src/cmdline.c
         src/selectrectangle.cpp
         src/glselectrectangle.cpp
         src/shader.cpp
         src/framebuffer.cpp
         src/resource.cpp
         src/xselectrectangle.cpp
         src/x.cpp
         src/main.cpp )
else()
    set( source
         src/cmdline.c
         src/selectrectangle.cpp
         src/xselectrectangle.cpp
         src/x.cpp
         src/main.cpp )
endif()

# Obtain library paths and make sure they exist.
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmakemodules" )
find_package( Imlib2    REQUIRED )
find_package( X11       REQUIRED )
find_package( XExt      REQUIRED )
find_package( OpenGL    REQUIRED )
find_package( GLX       REQUIRED )
find_package( XRender   REQUIRED )
find_package( XRandr    REQUIRED )
find_package( GLEW      REQUIRED )
# This library is needed only for Ubuntu it seems, some platforms don't even
# ship with it. I couldn't find a way to do a test compile to check if librt
# was needed, so instead I just didn't mark it as REQUIRED.
find_package( RT )

set( CMAKE_CXX_FLAGS
     "${CMAKE_CXX_FLAGS} ${CMAKE_IMLIB2_CXX_FLAGS}" )

# Includes
include_directories( "${X11_INCLUDE_DIR}"
                     "${XEXT_INCLUDE_DIR}" )

if ( CMAKE_OPENGL_SUPPORT )
include_directories( "${IMLIB2_INCLUDE_DIR}"
                     "${XRANDR_INCLUDE_DIR}"
                     "${OPENGL_INCLUDE_DIR}"
                     "${GLX_INCLUDE_DIR}"
                     ${GLEW_INCLUDE_DIRS}
                     ${XRENDER_INCLUDE_DIRS} )
endif()

if ( RT_INCLUDE_DIR )
    include_directories( ${RT_INCLUDE_DIR} )
endif()

# Executable
add_executable( "${BIN_TARGET}" ${source} )

# Libraries
target_link_libraries( "${BIN_TARGET}"
                       ${X11_LIBRARIES}
                       "${XEXT_LIBRARY}" )

if ( CMAKE_OPENGL_SUPPORT )
    target_link_libraries( "${BIN_TARGET}"
                           ${IMLIB2_LIBRARIES}
                           ${OPENGL_LIBRARIES}
                           ${GLX_LIBRARIES}
                           ${GLEW_LIBRARIES}
                           ${XRENDER_LIBRARIES}
                           "${XRANDR_LIBRARY}" )
endif()


if ( RT_LIBRARY )
    target_link_libraries( "${BIN_TARGET}"
                           "${RT_LIBRARY}" )
endif()

install( TARGETS ${BIN_TARGET}
         DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )

add_definitions(-DINSTALL_PREFIX="${INSTALL_PREFIX}")

if( CMAKE_OPENGL_SUPPORT )
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/share"
             DESTINATION "${CMAKE_INSTALL_PREFIX}" )
    add_definitions(-DOPENGL_ENABLED=1)
endif()
