
cmake_minimum_required(VERSION 3.22)

# CANopen library
add_library(canopen STATIC
    src/co.c
    src/sdo.c
    src/pdo.c
    src/nmt.c
    src/obj.c
    src/fifo.c
    src/filter.c
    src/heartbeat.c
)

target_include_directories(canopen PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)