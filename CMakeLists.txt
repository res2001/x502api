cmake_minimum_required(VERSION 2.6)

set(PROJECT x502api)
set(X502API_VER_MAJOR 0)
set(X502API_VER_MINOR 0)
set(X502API_VER_PATCH 2)
set(X502API_VERSION ${X502API_VER_MAJOR}.${X502API_VER_MINOR}.${X502API_VER_PATCH})

add_definitions(-DX502API_VER_MAJOR=${X502API_VER_MAJOR} -DX502API_VER_MINOR=${X502API_VER_MINOR} -DX502API_VER_PATCH=${X502API_VER_PATCH})

project(${PROJECT} C)


if(UNIX)
    set(CMAKE_PACKAGES ${CMAKE_CURRENT_SOURCE_DIR}/packages/packages.cmake)
    if(NOT OSC_PROJECT)
        set(OSC_PROJECT      home:l-card)
    endif(NOT OSC_PROJECT)
endif(UNIX)

set(X502API_LIB_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(LTIMER_DIR       ${X502API_LIB_DIR}/ltimer)
set(LCSPEC_DIR       ${X502API_LIB_DIR}/lcspec)

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

    #убираем флаг /INCREMENTAL, так как в MSVC с ним могут быть
    #внутренние ошибки линковщика
    foreach(flag_var
            CMAKE_SHARED_LINKER_FLAGS
            CMAKE_SHARED_LINKER_FLAGS_DEBUG
            CMAKE_SHARED_LINKER_FLAGS_RELEASE
            CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL
            CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO
            CMAKE_SHARED_EXE_FLAGS
            CMAKE_SHARED_EXE_FLAGS_DEBUG
            CMAKE_SHARED_EXE_FLAGS_RELEASE
            CMAKE_SHARED_EXE_FLAGS_MINSIZEREL
            CMAKE_SHARED_EXE_FLAGS_RELWITHDEBINFO)
       if(${flag_var} MATCHES "/INCREMENTAL:YES")
          string(REGEX REPLACE "/INCREMENTAL:YES" "/INCREMENTAL:NO" ${flag_var} "${${flag_var}}")
       endif(${flag_var} MATCHES "/INCREMENTAL:YES")
    endforeach(flag_var)
endif(MSVC)


include(${LTIMER_DIR}/ltimer.cmake)

set(SOURCES
    src/x502api.c
    src/x502api_config.c
    src/x502api_errs.c
    src/x502api_streams.c
    src/x502api_async.c
    src/x502api_bf.c
    src/x502api_eeprom.c
    lib/osspec/osspec.c
    lib/crc/fast_crc.c
    ${LTIMER_SOURCES})

set(HEADERS
    src/pstdint.h
    src/lstdtypes.h
    src/l502_bf_cmd_defs.h
    src/lboot_req.h
    src/x502_fpga_regs.h
    src/x502api.h
    src/x502api_private.h    
    src/x502_eeprom.h
    src/osspec_cfg.h
    lib/osspec/osspec.h
    src/fast_crc_cfg.h
    ${LTIMER_HEADERS})

include_directories(src lib/crc ${X502API_LIB_DIR}/osspec)


if(WIN32)
    #конфигурируем файл ресурсов для Windows
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/x502api.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/x502api.rc @ONLY)

    SET(SOURCES ${SOURCES} x502api.def ${CMAKE_CURRENT_BINARY_DIR}/x502api.rc)

    #подключение библиотеки для работы с сокетами
    set(LIBS ${LIBS} Ws2_32)
endif(WIN32)


add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})

#устанавливаем версию библиотеки для SO-файла
set_target_properties(${PROJECT} PROPERTIES VERSION ${X502API_VERSION})
set_target_properties(${PROJECT} PROPERTIES SOVERSION 1)


if(UNIX)
    set_target_properties(${PROJECT} PROPERTIES COMPILE_FLAGS "-fvisibility=hidden")
endif(UNIX)

target_link_libraries(${PROJECT} ${LIBS})


if(UNIX)
    install(TARGETS ${PROJECT} DESTINATION lib)
    install(FILES src/${PROJECT}.h src/lstdtypes.h src/pstdint.h DESTINATION include)
endif(UNIX)


add_subdirectory(src/devs)

#цели для сборки пакетов
if(UNIX AND CMAKE_PACKAGES)
    set(PACKAGE_NAME     ${PROJECT})
    set(PACKAGE_VERSION  ${X502API_VERSION})
    set(PACKAGE_ARCH_INSTALL_FILE ${PACKAGE_NAME}.install)
    include(${CMAKE_PACKAGES})
endif(UNIX AND CMAKE_PACKAGES)







