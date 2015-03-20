if(WIN32)
    #конфигурируем файл ресурсов для Windows
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc @ONLY)

    SET(SOURCES ${SOURCES} ${PROJECT_NAME}.def ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc)
endif(WIN32)

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES} ${SETUP_HEADERS})

target_link_libraries(${PROJECT} ${LIBS})

#устанавливаем версию библиотеки для SO-файла
set_target_properties(${PROJECT} PROPERTIES VERSION ${X502API_VERSION})
set_target_properties(${PROJECT} PROPERTIES SOVERSION 1)


if(UNIX)
    set_target_properties(${PROJECT} PROPERTIES COMPILE_FLAGS "-fvisibility=hidden")
endif(UNIX)


if(WIN32)
    #для Windows устанавливаем .lib файл отдельно от .dll в поддиректорию msvc
    install(TARGETS ${PROJECT} RUNTIME DESTINATION ${X502_INSTALL_LIB})
    install(TARGETS ${PROJECT} ARCHIVE DESTINATION ${X502_INSTALL_LIB}/msvc)
else()
    install(TARGETS ${PROJECT} DESTINATION ${X502_INSTALL_LIB})
endif()

install(FILES ${SETUP_HEADERS} DESTINATION ${X502_INSTALL_INCLUDE})
