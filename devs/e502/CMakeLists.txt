cmake_minimum_required(VERSION 2.6)

set(PROJECT e502api)
project(${PROJECT} C)



option(E502API_ENABLE_USB   "enable usb interface support" ON)
option(E502API_ENABLE_TCP   "enable tcp interface support" ON)
option(E502API_ENABLE_DNSSD "enable dns-sd service discovery" ON)

if(E502API_ENABLE_USB)
    option(E502API_LIBUSB_DEBUG "Print debug messages from libusb" OFF)
    if(WIN32)
        option(LIBUSB_INTERNAL "use internal libusb realisation" ON)
    endif(WIN32)
endif(E502API_ENABLE_USB)

if (E502API_ENABLE_DNSSD)
    if(WIN32)
        option(ENABLE_BONJOUR  "enable bonjour support" ON)
    else(WIN32)
        option(ENABLE_AVAHI  "enable avahi support" ON)
    endif(WIN32)
endif(E502API_ENABLE_DNSSD)





include(${LTIMER_DIR}/ltimer.cmake)

set(SOURCES
    e502api_usb.c
    e502api_tcp.c
    e502api_dnssd.c
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

include_directories(${X502API_LIB_DIR}/osspec)

if (E502API_ENABLE_USB)
    set(COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS} ENABLE_USB)

    if(E502API_LIBUSB_DEBUG)
        set(COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS} LIBUSB_DEBUG)
    endif(E502API_LIBUSB_DEBUG)


    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

    if(LIBUSB_INTERNAL)
        include(CheckStructHasMember)
        check_struct_has_member("struct timespec" tv_sec time.h HAVE_STRUCT_TIMESPEC LANGUAGE C)
        if(HAVE_STRUCT_TIMESPEC)
            add_definitions(-DHAVE_STRUCT_TIMESPEC)
        endif(HAVE_STRUCT_TIMESPEC)
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
endif(E502API_ENABLE_USB)


if(E502API_ENABLE_TCP)
    set(COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS} ENABLE_TCP)
    if(WIN32)
        #подключение библиотеки для работы с сокетами
        set(LIBS ${LIBS} Ws2_32)
    endif(WIN32)
endif(E502API_ENABLE_TCP)


if(E502API_ENABLE_DNSSD)
    set(COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS} ENABLE_DNSSD)

    if(ENABLE_BONJOUR)
        find_package(DNSSD REQUIRED)
        include_directories(${DNSSD_INCLUDE_DIRS})
        set(LIBS ${LIBS} ${DNSSD_LIBRARIES})
        set(COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS} ENABLE_BONJOUR)
    endif(ENABLE_BONJOUR)

    if(ENABLE_AVAHI)
        find_package(Avahi REQUIRED)
        include_directories(${AVAHI_INCLUDE_DIRS})
        set(LIBS ${LIBS} ${AVAHI_LIBRARIES})
        set(COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS} ENABLE_AVAHI)
    endif(ENABLE_AVAHI)
endif(E502API_ENABLE_DNSSD)



include(${X502_LIBS_CMAKE_FILE})


if(UNIX)
    if (E502API_ENABLE_USB)
        if(NOT UDEV_RULES_DIR)
            set(UDEV_RULES_DIR lib/udev/rules.d)
        endif(NOT UDEV_RULES_DIR)

        install(FILES e502.rules DESTINATION ${UDEV_RULES_DIR})
     endif(E502API_ENABLE_USB)
endif(UNIX)






