cmake_minimum_required(VERSION 2.6)

set(PROJECT e502api)
project(${PROJECT} C)


include(${LTIMER_DIR}/ltimer.cmake)

set(SOURCES
    e502api_usb.c
    e502api_tcp.c
    e502api.c
    e502api_eth_config.c
    ${LTIMER_SOURCES}
    ${X502API_LIB_DIR}/osspec/osspec.c
    )
    
set(HEADERS
    ${LTIMER_HEADERS}
    e502_fpga_regs.h
    e502_cm4_defs.h
    e502_eth_config.h
    e502api.h
    e502api_private.h
    e502_tcp_protocol.h)

set(LIBS
    x502api
    ${LTIMER_LIBS})


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
#ищем libusb-1.0
find_package(LibUSB REQUIRED)
include_directories(${LIBUSB_INCLUDE_DIR})
set(LIBS ${LIBS} ${LIBUSB_LIBRARIES})


include_directories(${X502API_LIB_DIR}/osspec)


add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})

#устанавливаем версию библиотеки для SO-файла
set_target_properties(${PROJECT} PROPERTIES VERSION ${X502API_VERSION})
set_target_properties(${PROJECT} PROPERTIES SOVERSION 1)

if(UNIX)
    set_target_properties(${PROJECT} PROPERTIES COMPILE_FLAGS "-fvisibility=hidden")
endif(UNIX)

target_link_libraries(${PROJECT} ${LIBS})

if(UNIX)
    if(NOT UDEV_RULES_DIR)
        set(UDEV_RULES_DIR lib/udev/rules.d)
    endif(NOT UDEV_RULES_DIR)

    install(TARGETS ${PROJECT} DESTINATION lib)
    install(FILES e502api.h DESTINATION include)
    install(FILES e502.rules DESTINATION ${UDEV_RULES_DIR})
endif(UNIX)






