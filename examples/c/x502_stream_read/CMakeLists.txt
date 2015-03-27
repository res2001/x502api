# Данный файл описывает проект для системы сборки cmake.
# С помощью cmake из этого файла можно получить нужный make-файл
# для нужно родной системы сборки или проект для нужной среды.
# 
# Для этого, из созданной директории для сборки нужно вызвать:
#
# cmake -DX502API_INCLUDE_DIR=<путь к заголовочным файлам x502api> -DX502API_LIBRARIES_DIR=<путь к файлу линкера x502api (.lib, .a или .so)> -G <имя генератора проекта> <путь к исходникам (данному файлу)>
# 
# списко поддерживаемых генераторов можно получить вызвав cmake без аргументов 
#
# Если заголовки и библиотека/файл линкера лежат в стандартных директориях, 
#   то -DX502API_INCLUDE_DIR и -DX502API_LIBRARIES_DIR можно соответственно не указывать
# 

cmake_minimum_required(VERSION 2.6)

set(PROJECT x502_stream_read)

project(${PROJECT} C)

set(SOURCES main.c)
set(HEADERS )

include_directories(${X502API_INCLUDE_DIR})
link_directories(${X502API_LIBRARIES_DIR})

add_executable(${PROJECT} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT} x502api l502api e502api)
