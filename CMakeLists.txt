cmake_minimum_required(VERSION 2.6)

set(PROJECT x502api)
set(X502API_VER_MAJOR 0)
set(X502API_VER_MINOR 0)
set(X502API_VER_PATCH 1)
set(X502API_VERSION ${L502API_VER_MAJOR}.${L502API_VER_MINOR}.${L502API_VER_PATCH})

add_definitions(-DX502API_VER_MAJOR=${L502API_VER_MAJOR} -DX502API_VER_MINOR=${L502API_VER_MINOR} -DX502API_VER_PATCH=${L502API_VER_PATCH})

project(${PROJECT} C)

#для GCC устанавливаем повышенный уроень предупреждения компилятора
if(CMAKE_COMPILER_IS_GNUCC)
    set(WARNOPTS
        -Werror=implicit-int -Werror=implicit-function-declaration -Werror=strict-prototypes -Werror=return-type
        -Wall -Wextra
        -Wformat-security -Winit-self -Wstrict-aliasing -Wfloat-equal
        -Wundef -Wshadow -Wunsafe-loop-optimizations -Wpointer-arith -Wcast-align
        -Wwrite-strings  -Wsign-compare -Wlogical-op -Waggregate-return -Winline
        -Wno-unused-parameter -Wno-unused-variable -Wno-aggregate-return)
    add_definitions(${WARNOPTS})

endif(CMAKE_COMPILER_IS_GNUCC)

#при сборке MSVC компилируем runtime-библиотеку статически, чтобы не зависеть от версии
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    foreach(flag_var
            CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
       if(${flag_var} MATCHES "/MD")
          string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
       endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)
endif(MSVC)


set(SOURCES
    src/e502/e502api_usb.c
    src/e502/e502api.c
    src/x502api.c
    src/x502_config.c
    src/x502_errs.c
    lib/osspec/osspec.c
    lib/crc/fast_crc.c)

set(HEADERS
    src/pstdint.h
    src/lpcie.h
    src/l502_bf_cmd_defs.h
    src/lboot_req.h
    src/x502_fpga_regs.h
    src/x502api.h
    src/x502api_private.h    

    src/e502/e502_fpga_regs.h
    src/e502/e502_cm4_defs.h    
    src/e502/e502api.h
    
    src/osspec_cfg.h
    lib/osspec/osspec.h
    src/fast_crc_cfg.h)

include_directories(src lib/crc lib/osspec)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
#для linux используем libusb-1.0
if(UNIX)
    #ищем libusb
    find_package(LibUSB REQUIRED)
    include_directories(${LIBUSB_INCLUDE_DIR})
    set(LIBS ${LIBS} ${LIBUSB_LIBRARIES})
endif(UNIX)



#Включаем файлы для таймера и inline-функций
set(HEADERS ${HEADERS} lib/timer/timer.h lib/lcspec/lcspec.h)
include_directories(lib/timer lib/lcspec)
if(UNIX)
    set(SOURCES ${SOURCES} lib/timer/ports/linux/clock.c)
    include_directories(lib/timer/ports/linux)
elseif(WIN32)
    set(SOURCES ${SOURCES} lib/timer/ports/win/clock.c)
    include_directories(lib/timer/ports/win)
endif(UNIX)


add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})

#устанавливаем версию библиотеки для SO-файла
set_target_properties(${PROJECT} PROPERTIES VERSION ${X502API_VERSION})
set_target_properties(${PROJECT} PROPERTIES SOVERSION 1)


if(UNIX)
    target_link_libraries(${PROJECT} rt)
    set_target_properties(${PROJECT} PROPERTIES COMPILE_FLAGS "-fvisibility=hidden")
endif(UNIX)

target_link_libraries(${PROJECT} ${LIBS})

if(UNIX)
    install(TARGETS ${PROJECT} DESTINATION lib)
    install(FILES src/${PROJECT}.h src/lpcie.h src/pstdint.h
            DESTINATION include)
endif(UNIX)
