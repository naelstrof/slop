# - Find XExt
# Find the XExtension libraries (WARNING: It's geared and tested specifically for the XShape extension though)
#
#  This module defines the following variables:
#     XEXT_FOUND        - 1 if XEXT_INCLUDE_DIR & XEXT_LIBRARY are found, 0 otherwise
#     XEXT_INCLUDE_DIR  - where to find Xlib.h, etc.
#     XEXT_LIBRARY      - the X11 library
#

find_path( XEXT_INCLUDE_DIR
           NAMES X11/extensions/shape.h
           PATH_SUFFIXES X11/extensions
           DOC "The XExtension include directory" )

find_library( XEXT_LIBRARY
              NAMES Xext
              PATHS /usr/lib /lib
              DOC "The XExtension library" )

if( XEXT_INCLUDE_DIR AND XEXT_LIBRARY )
    set( XEXT_FOUND 1 )
else()
    set( XEXT_FOUND 0 )
endif()

mark_as_advanced( XEXT_INCLUDE_DIR XEXT_LIBRARY )
