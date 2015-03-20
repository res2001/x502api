cmake_minimum_required(VERSION 2.6)

set(PROJECT l502api)
project(${PROJECT} C)


include(${LTIMER_DIR}/ltimer.cmake)

set(SOURCES
    l502api.c
    l502api_compat.c
    l502api_eeprom.c
    l502api_bf.c
    ${LTIMER_SOURCES}
    )
    
set(HEADERS
    l502api.h
    l502api_compat.h
    l502api_private.h
    ${LTIMER_HEADERS})

set(LIBS
    x502api
    ${LTIMER_LIBS})


if(WIN32)
    #конфигурируем файл ресурсов для Windows
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/win/l502api.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/l502api.rc @ONLY)

    set(SOURCES ${SOURCES} win/l502_spec.c win/l502api.def ${CMAKE_CURRENT_BINARY_DIR}/l502api.rc)

    #В Windows используем setupapi для нахождения устройства
    set(LIBS ${LIBS} setupapi)
elseif(UNIX)
    set(SOURCES ${SOURCES} linux/l502_spec.c)
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
    install(FILES l502api.h l502api_compat.h DESTINATION include)
endif(UNIX)






