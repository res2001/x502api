# - Try to find dnssd library from Bonjour SDK
# Once done this will define
#
#  DNSSD_FOUND - system has dnssd library
#  DNSSD_INCLUDE_DIRS - the dnssd include directory
#  DNSSD_LIBRARIES - Link these to use dnssd library

if (DNSSD_INCLUDE_DIRS AND DNSSD_LIBRARIES)

  # in cache already
  set(DNSSD_FOUND TRUE)
  message(STATUS "Found dnssd: ${DNSSD_LIBRARIES}, ${DNSSD_INCLUDE_DIRS}")

else (DNSSD_INCLUDE_DIRS AND DNSSD_LIBRARIES)

  find_path(DNSSD_INCLUDE_DIRS dns_sd.h
    PATHS ENV PROGRAMFILES ENV PROGRAMFILES(x86)
    PATH_SUFFIXES "Bonjour SDK/Include"
  )

  if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
    set(DNSSD_LIBRARY_PATH_SUFFIX "Bonjour SDK/Lib/Win32")
  else()
    set(DNSSD_LIBRARY_PATH_SUFFIX "Bonjour SDK/Lib/x64")
  endif()

  find_library(DNSSD_LIBRARIES NAMES dnssd
    PATHS ENV PROGRAMFILES ENV PROGRAMFILES(x86)
    PATH_SUFFIXES ${DNSSD_LIBRARY_PATH_SUFFIX}

  )

 set(CMAKE_REQUIRED_INCLUDES ${DNSSD_INCLUDE_DIRS})
 set(CMAKE_REQUIRED_LIBRARIES ${DNSSD_LIBRARIES})

  if(DNSSD_INCLUDE_DIRS AND DNSSD_LIBRARIES)
    set(DNSSD_FOUND TRUE)
  else (DNSSD_INCLUDE_DIRS AND DNSSD_LIBRARIES)
    set(DNSSD_FOUND FALSE)
  endif(DNSSD_INCLUDE_DIRS AND DNSSD_LIBRARIES)

  if (DNSSD_FOUND)
    if (NOT DNSSD_FIND_QUIETLY)
      message(STATUS "Found dnssd: ${DNSSD_LIBRARIES}, ${DNSSD_INCLUDE_DIRS}")
    endif (NOT DNSSD_FIND_QUIETLY)
  else (DNSSD_FOUND)
    if (DNSSD_FIND_REQUIRED)
      message(FATAL_ERROR "dnssd not found!")
    endif (DNSSD_FIND_REQUIRED)
  endif (DNSSD_FOUND)

  mark_as_advanced(DNSSD_INCLUDE_DIRS DNSSD_LIBRARIES)
  
endif (DNSSD_INCLUDE_DIRS AND DNSSD_LIBRARIES)
