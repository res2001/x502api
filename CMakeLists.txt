cmake_minimum_required(VERSION 2.6)

set(PROJECT x502api)
set(X502API_VER_MAJOR 1)
set(X502API_VER_MINOR 0)
set(X502API_VER_PATCH 99)
set(X502API_VERSION ${X502API_VER_MAJOR}.${X502API_VER_MINOR}.${X502API_VER_PATCH})

add_definitions(-DX502API_VER_MAJOR=${X502API_VER_MAJOR} -DX502API_VER_MINOR=${X502API_VER_MINOR} -DX502API_VER_PATCH=${X502API_VER_PATCH})


if(WIN32)
    option(LTRAPI_BUILD_BORLAND_LIBS "Make .lib files for Borland C++/C++ Builder compiler" OFF)
    option(LTRAPI_BUILD_MINGW_LIBS "Make .a files for mingw compiler" OFF)
endif(WIN32)


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
set(X502_LIBS_CMAKE_FILE  ${CMAKE_CURRENT_SOURCE_DIR}/x502libs.cmake)

set(X502_INSTALL_LIB        lib)
set(X502_INSTALL_INCLUDE    include)

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


set(SETUP_HEADERS
    src/lcard_pstdint.h
    src/x502api.h
    )

set(HEADERS    
    src/l502_bf_cmd_defs.h
    src/lboot_req.h
    src/x502_fpga_regs.h
    src/x502api_private.h
    src/x502_eeprom.h
    src/osspec_cfg.h
    lib/osspec/osspec.h
    src/fast_crc_cfg.h
    ${LTIMER_HEADERS})



include_directories(src lib/crc ${X502API_LIB_DIR}/osspec)


include(${X502_LIBS_CMAKE_FILE})


add_subdirectory(devs)


if(LPCIE_DOCGEN_DIR)
    include(${LPCIE_DOCGEN_DIR}/LDoxyToPdf.cmake)
    set(PDF_IMG_FILES  doc/images/adc_frame.jpeg)
    set(DOC_DOXYGEN_PRJ_FILE doc/Doxyfile.in)
    set(DOXYGEN_INPUT_FILES devs/l502/l502api.h devs/e502/e502api.h src/x502api.h  doc/mainpage.md doc/about.md doc/setup.md doc/gen_descr.md)
    ADD_DOXY_TO_PDF_TARGET(x502api)
endif(LPCIE_DOCGEN_DIR)

#цели для сборки пакетов
if(UNIX AND CMAKE_PACKAGES)
    set(PACKAGE_NAME     ${PROJECT})
    set(PACKAGE_VERSION  ${X502API_VERSION})
    set(PACKAGE_ARCH_INSTALL_FILE ${PACKAGE_NAME}.install)
    include(${CMAKE_PACKAGES})
endif(UNIX AND CMAKE_PACKAGES)



if(WIN32 AND LCARD_SDK_MAKE_INSTALLER)
    include(win_install/winpaths.cmake)

    set(X502API_CONVERTED_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/conv_src)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/x64)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/x86)
    file(MAKE_DIRECTORY ${X502API_CONVERTED_SOURCE_DIR})

    string(REGEX REPLACE "/" "\\\\" ICONV ${ICONV})
    string(REGEX REPLACE "/" "\\\\" X502API_CONVERTED_SOURCE_DIR ${X502API_CONVERTED_SOURCE_DIR})
    set(X502API_ORIG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    string(REGEX REPLACE "/" "\\\\" X502API_ORIG_SOURCE_DIR ${X502API_ORIG_SOURCE_DIR})

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/win_install/conv_src_cp1251.bat.in
                   ${CMAKE_CURRENT_BINARY_DIR}/conv_src_cp1251.bat @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/win_install/make64.bat.in
               ${CMAKE_CURRENT_BINARY_DIR}/x64/make.bat @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/win_install/make32.bat.in
               ${CMAKE_CURRENT_BINARY_DIR}/x86/make.bat @ONLY)
    add_custom_target(convert_src
        COMMAND cmd /c conv_src_cp1251.bat
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_custom_target(x502api_64
        COMMAND cmd /c make.bat
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/x64
        DEPENDS convert_src)

    #x86 версию собираем 2008 студией (Express) для совместимости с Win2000
    #set(VS2008_SETENV "${VS2008_DIR}/vcvarsall.bat")
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/x86)
    add_custom_target(x502api_32
        COMMAND cmd /c make.bat
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/x86
        DEPENDS convert_src
        )
endif()



