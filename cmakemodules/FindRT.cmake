# - Find rt
# Find the rt libraries, this is to fix ubuntu not having clock_gettime defined without it.
#
#  This module defines the following variables:
#     RT_FOUND        - 1 if RT_INCLUDE_DIR & RT_LIBRARY are found, 0 otherwise
#     RT_INCLUDE_DIR  - where to find Xlib.h, etc.
#     RT_LIBRARY      - the X11 library
#

find_path( RT_INCLUDE_DIR
           NAMES time.h )

find_library( RT_LIBRARY
              NAMES rt
              PATHS /usr/lib /lib )

if( RT_INCLUDE_DIR AND RT_LIBRARY )
    set( RT_FOUND 1 )
else()
    set( RT_FOUND 0 )
endif()

mark_as_advanced( RT_INCLUDE_DIR RT_LIBRARY )
