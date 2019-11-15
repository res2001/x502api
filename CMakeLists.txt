cmake_minimum_required(VERSION 2.6)

set(PROJECT x502api)
set(X502API_VER_MAJOR 1)
set(X502API_VER_MINOR 1)
set(X502API_VER_PATCH 13)
set(X502API_VERSION ${X502API_VER_MAJOR}.${X502API_VER_MINOR}.${X502API_VER_PATCH})

add_definitions(-DX502API_VER_MAJOR=${X502API_VER_MAJOR} -DX502API_VER_MINOR=${X502API_VER_MINOR} -DX502API_VER_PATCH=${X502API_VER_PATCH})

option(X502API_ENABLE_DEV_L502 "Enable L-502 device support" ON)
option(X502API_ENABLE_DEV_E502 "Enable E-502 device support" ON)


if(WIN32)
    option(LTRAPI_BUILD_BORLAND_LIBS "Make .lib files for Borland C++/C++ Builder compiler" OFF)
    option(LTRAPI_BUILD_MINGW_LIBS "Make .a files for mingw compiler" OFF)
endif(WIN32)


project(${PROJECT} C)



set(X502API_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
set(X502API_LIB_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(LTIMER_DIR       ${X502API_LIB_DIR}/ltimer)
set(LCSPEC_DIR       ${X502API_LIB_DIR}/lcspec)
set(OSSPEC_DIR       ${X502API_LIB_DIR}/osspec)
set(X502_LIBS_CMAKE_FILE  ${CMAKE_CURRENT_SOURCE_DIR}/x502libs.cmake)

if(NOT X502_INSTALL_LIB)
    set(X502_INSTALL_LIB        lib)
endif(NOT X502_INSTALL_LIB)
if (NOT X502_INSTALL_INCLUDE)
    set(X502_INSTALL_INCLUDE    include)
endif(NOT X502_INSTALL_INCLUDE)
set(X502_INSTALL_PASCAL     pas)


file(MAKE_DIRECTORY ${X502API_INCLUDE_DIR})
set(X502API_PREPARE_HEADERS ON)

#для GCC устанавливаем повышенный уроень предупреждения компилятора
if(CMAKE_COMPILER_IS_GNUCC)
    set(WARNOPTS)
    include(CheckCCompilerFlag)
    set(CHECK_FLAGS
        -Wall -Wformat-security -Wundef -Wshadow -Wpointer-arith -Wcast-align
        -Wwrite-strings  -Wsign-compare -Waggregate-return -Winline -Wno-aggregate-return
        -Werror=implicit-int -Werror=implicit-function-declaration -Werror=strict-prototypes -Werror=return-type
        -Wextra -Winit-self -Wstrict-aliasing -Wfloat-equal
        -Wunsafe-loop-optimizations -Wlogical-op
        -Wno-unused-parameter -Wno-unused-variable)
    foreach(CUR_FLAG ${CHECK_FLAGS})
        unset(FLAG_ENABLE CACHE)
        CHECK_C_COMPILER_FLAG(${CUR_FLAG} FLAG_ENABLE)
        if(FLAG_ENABLE)
            set(WARNOPTS ${WARNOPTS} ${CUR_FLAG})
        endif(FLAG_ENABLE)
    endforeach(CUR_FLAG)

    unset(FLAG_NO_UNDEFINED CACHE)
    CHECK_C_COMPILER_FLAG(-Wl,--no-undefined FLAG_NO_UNDEFINED)
    if(FLAG_NO_UNDEFINED)
        set(LINK_FLAGS -Wl,--no-undefined)
    else(FLAG_NO_UNDEFINED)
        message(STATUS "LINKER FLAG -Wl,--no-undefined WAS NOT FOUND!")
    endif(FLAG_NO_UNDEFINED)

    unset(FLAG_FVISIBILITY CACHE)
    CHECK_C_COMPILER_FLAG(-fvisibility=hidden FLAG_FVISIBILITY)

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
set(NO_OSSPEC_USE_EVENTS ON)
set(NO_OSSPEC_USE_THREADS ON)
include(${OSSPEC_DIR}/osspec.cmake)
set(COMPILE_DEFINITIONS ${LTIMER_DEFINITIONS} ${OSSPEC_DEFINITIONS})

set(SOURCES
    src/x502api.c
    src/x502api_config.c
    src/x502api_errs.c
    src/x502api_streams.c
    src/x502api_async.c
    src/x502api_bf.c
    src/x502api_eeprom.c
    lib/crc/fast_crc.c
    ${OSSPEC_SOURCES}
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
    src/fast_crc_cfg.h
    ${OSSPEC_HEADERS}
    ${LTIMER_HEADERS})



include_directories(src lib/crc ${X502API_LIB_DIR}/osspec)


include(${X502_LIBS_CMAKE_FILE})


add_subdirectory(devs)

if(BUILD_EXAMPLES OR BUILD_ALL)
    add_subdirectory(examples/c)
endif(BUILD_EXAMPLES OR BUILD_ALL)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif(BUILD_TESTS)

if(LPCIE_DOCGEN_DIR)
    include(${LPCIE_DOCGEN_DIR}/LDoxyToPdf.cmake)
    set(PDF_IMG_FILES  doc/images/adc_frame.jpeg)
    set(DOC_DOXYGEN_PRJ_FILE doc/Doxyfile.in)
    set(DOXYGEN_INPUT_FILES devs/l502/l502api.h devs/e502/e502api.h src/x502api.h  doc/mainpage.md doc/about.md doc/setup.md doc/gen_descr.md)
    ADD_DOXY_TO_PDF_TARGET(x502api)
endif(LPCIE_DOCGEN_DIR)

#цели для сборки пакетов
if(UNIX AND CMAKE_EXT_INCLUDE_DIR)
    set(PACKAGE_NAME     ${PROJECT})
    set(PACKAGE_VERSION  ${X502API_VERSION})
    set(PACKAGE_ARCH_INSTALL_FILE ${PACKAGE_NAME}.install)
    include(${CMAKE_EXT_INCLUDE_DIR}/packages/packages.cmake)
endif(UNIX AND CMAKE_EXT_INCLUDE_DIR)



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



