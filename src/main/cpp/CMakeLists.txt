cmake_minimum_required(VERSION 3.18)
project(RTKLIB)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -fPIC")

file(GLOB RTKLIB_SOURCES
    "rtklib_2_4_3/*.c"
    "rtklib_2_4_3/rcv/*.c"
    "rtklib_jni.c"
)

add_library(rtklib SHARED ${RTKLIB_SOURCES})

target_include_directories(rtklib PRIVATE "${CMAKE_SOURCE_DIR}/rtklib_2_4_3")
target_link_libraries(rtklib log android)
