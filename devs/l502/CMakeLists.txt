cmake_minimum_required(VERSION 2.6)

project(l502api C)
set(PROJECT_VARNAME_PREFIX L502API)



include(${LTIMER_DIR}/ltimer.cmake)

set(SOURCES
    l502api.c
    l502api_compat.c
    l502api_eeprom.c
    l502api_bf.c
    ${LTIMER_SOURCES}
    )
    
set(SETUP_HEADERS
    l502api.h
    l502api_compat.h)

set(HEADERS
    l502api_private.h
    lpcie_ioctls.h
    ${LTIMER_HEADERS})

set(LIBS
    x502api
    ${LTIMER_LIBS})

set(L502API_COMPILE_DEFINITIONS ${L502API_COMPILE_DEFINITIONS} ${LTIMER_DEFINITIONS})

if(WIN32)
    #В Windows используем setupapi для нахождения устройства
    set(LIBS ${LIBS} setupapi)
    set(SOURCES ${SOURCES} win/l502_spec.c)
elseif(UNIX)
    set(SOURCES ${SOURCES} linux/l502_spec.c)
endif(WIN32)

include(${X502_LIBS_CMAKE_FILE})







