cmake_minimum_required(VERSION 2.6)

set(PROJECT e502api)
project(${PROJECT} C)

option(E502API_LIBUSB_DEBUG "Print debug messages from libusb" OFF)

include(${LTIMER_DIR}/ltimer.cmake)

set(SOURCES
    e502api_usb.c
    e502api_tcp.c
    e502api.c
    e502api_eth_config.c
    ${LTIMER_SOURCES}
    ${X502API_LIB_DIR}/osspec/osspec.c
    )

set(SETUP_HEADERS e502api.h)
    
set(HEADERS
    ${LTIMER_HEADERS}
    e502_fpga_regs.h
    e502_cm4_defs.h
    e502_eth_config.h
    e502api_private.h
    e502_tcp_protocol.h)

set(LIBS
    x502api
    ${LTIMER_LIBS})

if(E502API_LIBUSB_DEBUG)
    add_definitions(-DLIBUSB_DEBUG)
endif(E502API_LIBUSB_DEBUG)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
#ищем libusb-1.0
find_package(LibUSB REQUIRED)
include_directories(${LIBUSB_INCLUDE_DIR})
set(LIBS ${LIBS} ${LIBUSB_LIBRARIES})


include_directories(${X502API_LIB_DIR}/osspec)

if(WIN32)
    #подключение библиотеки для работы с сокетами
    set(LIBS ${LIBS} Ws2_32)
endif(WIN32)

include(${X502_LIBS_CMAKE_FILE})


if(UNIX)
    if(NOT UDEV_RULES_DIR)
        set(UDEV_RULES_DIR lib/udev/rules.d)
    endif(NOT UDEV_RULES_DIR)

    install(FILES e502.rules DESTINATION ${UDEV_RULES_DIR})
endif(UNIX)






