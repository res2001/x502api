cmake_minimum_required(VERSION 2.6)

set(PROJECT e502api)
project(${PROJECT} C)

option(E502API_LIBUSB_DEBUG "Print debug messages from libusb" OFF)

if(WIN32)
    option(LIBUSB_INTERNAL "use internal libusb realisation" ON)
    option(ENABLE_BONJOUR  "enable bonjour support" ON)
else(WIN32)
    option(ENABLE_AVAHI  "enable avahi support" ON)
endif(WIN32)

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
    e502api_tcp_private.h
    e502_tcp_protocol.h)

set(LIBS
    x502api
    ${LTIMER_LIBS})

if(E502API_LIBUSB_DEBUG)
    add_definitions(-DLIBUSB_DEBUG)
endif(E502API_LIBUSB_DEBUG)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

if(LIBUSB_INTERNAL) 
    # использование внутренней реализации libusb, а не внешеней библиотеки
    # используется под windows, так как стандартная версия c WinUSB драйвером
    # имеет проблемны при отмене трансферов
    include_directories(libusb-1.0 libusb-1.0/msvc)
    set(SOURCES ${SOURCES}
        libusb-1.0/core.c
        libusb-1.0/descriptor.c
        libusb-1.0/hotplug.c
        libusb-1.0/io.c
        libusb-1.0/strerror.c
        libusb-1.0/sync.c
        libusb-1.0/os/poll_windows.c
        libusb-1.0/os/threads_windows.c
        libusb-1.0/os/windows_usb.c
        )

    set(HEADERS ${HEADERS}
        libusb-1.0/libusb.h
        libusb-1.0/libusbi.h
        libusb-1.0/hotplug.h
        libusb-1.0/version.h
        libusb-1.0/version_nano.h
        libusb-1.0/os/poll_windows.h
        libusb-1.0/os/threads_windows.h
        libusb-1.0/os/windows_common.h
        libusb-1.0/msvc/config.h
        libusb-1.0/msvc/errno.h
        libusb-1.0/msvc/inttypes.h
        libusb-1.0/msvc/missing.h
        libusb-1.0/msvc/stdint.h
        )
else()
    #ищем libusb-1.0
    find_package(LibUSB REQUIRED)
    include_directories(${LIBUSB_INCLUDE_DIR})
    set(LIBS ${LIBS} ${LIBUSB_LIBRARIES})
endif()

if(ENABLE_BONJOUR)
    find_package(DNSSD REQUIRED)
    include_directories(${DNSSD_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${DNSSD_LIBRARIES})
    set(SOURCES ${SOURCES} e502api_dnssd.c)
    add_definitions("-DENABLE_BONJOUR -DENABLE_DNSSD")
endif(ENABLE_BONJOUR)

if(ENABLE_AVAHI)
    find_package(Avahi REQUIRED)
    include_directories(${AVAHI_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${AVAHI_LIBRARIES})
    set(SOURCES ${SOURCES} e502api_dnssd.c)
    add_definitions("-DENABLE_AVAHI -DENABLE_DNSSD")
endif(ENABLE_AVAHI)


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






