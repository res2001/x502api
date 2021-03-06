

if(WIN32)
    #конфигурируем файл ресурсов для Windows
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc @ONLY)

    SET(SOURCES ${SOURCES} ${PROJECT_NAME}.def ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc)
endif(WIN32)

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES} ${SETUP_HEADERS})

target_link_libraries(${PROJECT_NAME} ${LIBS})

#устанавливаем версию библиотеки для SO-файла
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${X502API_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)

set(PRJ_COMPILE_FLAGS ${COMPILE_FLAGS}  ${${PROJECT_VARNAME_PREFIX}_COMPILE_FLAGS})
if(PRJ_COMPILE_FLAGS)
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "${PRJ_COMPILE_FLAGS}")
endif(PRJ_COMPILE_FLAGS)
message("${PROJECT_NAME} compile flags: ${PRJ_COMPILE_FLAGS}")

set(PRJ_COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS} ${${PROJECT_VARNAME_PREFIX}_COMPILE_DEFINITIONS})
if (PRJ_COMPILE_DEFINITIONS)
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS "${PRJ_COMPILE_DEFINITIONS}")
endif(PRJ_COMPILE_DEFINITIONS)
message("${PROJECT_NAME} compile definitions: ${PRJ_COMPILE_DEFINITIONS}")

set(PRJ_LINK_FLAGS ${LINK_FLAGS} ${${PROJECT_NAME}_LINK_FLAGS})
if(PRJ_LINK_FLAGS)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "${PRJ_LINK_FLAGS}")
endif(PRJ_LINK_FLAGS)

if(WIN32)
    #для Windows устанавливаем .lib файл отдельно от .dll в поддиректорию msvc
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${X502_INSTALL_LIB})
    install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION ${X502_INSTALL_LIB}/msvc)
else()
    install(TARGETS ${PROJECT_NAME} DESTINATION ${X502_INSTALL_LIB})
endif()

install(FILES ${SETUP_HEADERS} DESTINATION ${X502_INSTALL_INCLUDE})

if(WIN32)
    install(FILES pas/${PROJECT_NAME}.pas DESTINATION ${X502_INSTALL_PASCAL})
endif(WIN32)

if(X502API_BUILD_BORLAND_LIBS)
    #создание цели для создания библиотеки для borland C++/C++ Builder
    set(MODULE_BORLAND  ${PROJECT_NAME}_borland)
    #создаем lib-файл для Borland C++ (лучше использовать последнюю версию,
    #так как lib созданный с помощью Borland Builder 6 может не подойти к Embarcadero)

    if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
        #для 32-битных библиотек используем implib и файл с расширением .lib
        set(MODULE_BORLAND_EXTENSION lib)
        set(MODULE_BORLAND_LIBFILE   ${MODULE_BORLAND}.${MODULE_BORLAND_EXTENSION})
        add_custom_command(OUTPUT ${MODULE_BORLAND_LIBFILE} ALL
            COMMAND "${BORLAND_IMPLIB}" -f ${MODULE_BORLAND} ${PROJECT_NAME}.dll
            DEPENDS ${PROJECT_NAME}
            COMMENT "make borland library for ${PROJECT_NAME}")
    else( CMAKE_SIZEOF_VOID_P EQUAL 4 )
        #для 64-битных библиотек используем mkexp и файл с расширением .a
        set(MODULE_BORLAND_EXTENSION a)
        set(MODULE_BORLAND_LIBFILE   ${MODULE_BORLAND}.${MODULE_BORLAND_EXTENSION})
        add_custom_command(OUTPUT ${MODULE_BORLAND_LIBFILE} ALL
            COMMAND "${BORLAND_MKEXP}" ${MODULE_BORLAND_LIBFILE} ${PROJECT_NAME}.dll
            DEPENDS ${PROJECT_NAME}
            COMMENT "make 64-bit borland library for ${PROJECT_NAME}")
    endif( CMAKE_SIZEOF_VOID_P EQUAL 4 )

    add_custom_target(${MODULE_BORLAND} ALL
                    DEPENDS ${MODULE_BORLAND_LIBFILE})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_BORLAND_LIBFILE}
            DESTINATION ${X502_INSTALL_LIB}/borland
            RENAME ${PROJECT_NAME}.${MODULE_BORLAND_EXTENSION})
endif(X502API_BUILD_BORLAND_LIBS)


if(X502API_BUILD_MINGW_LIBS)
    set(MODULE_MINGW           ${PROJECT_NAME}_mingw)
    set(MODULE_MINGW_LIBFILE   ${MODULE_MINGW}.a)

    if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
        set(MODULE_MINGW_MACHINE  i386)
    else( CMAKE_SIZEOF_VOID_P EQUAL 4 )
        set(MODULE_MINGW_MACHINE  i386:x86-64)
    endif( CMAKE_SIZEOF_VOID_P EQUAL 4 )

    add_custom_command(OUTPUT ${MODULE_MINGW_LIBFILE} ALL
        COMMAND "${GENDEF}" - -a  ${PROJECT_NAME}.dll > ${MODULE_MINGW}_tmp.def
        COMMAND "${DLLTOOL}" -m ${MODULE_MINGW_MACHINE} -l ${MODULE_MINGW_LIBFILE} -k -d ${MODULE_MINGW}_tmp.def
        DEPENDS ${PROJECT_NAME}
        COMMENT "make mingw library for ${PROJECT_NAME}")
    add_custom_target(${MODULE_MINGW} ALL
                    DEPENDS ${MODULE_MINGW_LIBFILE})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_MINGW_LIBFILE}
            DESTINATION ${X502_INSTALL_LIB}/mingw
            RENAME lib${PROJECT_NAME}.a)
endif(X502API_BUILD_MINGW_LIBS)


if(X502API_PREPARE_HEADERS)
    foreach(PREP_FILE ${SETUP_HEADERS})
        get_filename_component(PREP_FILENAME ${PREP_FILE} NAME)
        add_custom_command(TARGET ${PROJECT_NAME}
                           POST_BUILD
                           COMMAND cmake -E copy "${PREP_FILE}" "${X502API_INCLUDE_DIR}/${PREP_FILENAME}"
                           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                          )
    endforeach()
endif(X502API_PREPARE_HEADERS)
