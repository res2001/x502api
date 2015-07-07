cmake_minimum_required(VERSION 2.6)

set(PROJECT x502_rdy_cntr_tst)

project(${PROJECT} C)

set(SOURCES main.c)
set(HEADERS )

include_directories(${X502API_INCLUDE_DIR})
link_directories(${X502API_LIBRARIES_DIR})

add_executable(${PROJECT} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT} x502api l502api e502api)
