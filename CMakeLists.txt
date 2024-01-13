cmake_minimum_required(VERSION 3.14.5)
project(HelloAndroidNative)

include_directories(
    "${CMAKE_SOURCE_DIR}/glue/"
    "${CMAKE_SOURCE_DIR}/src/"
)

add_library(white_screen SHARED 
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_SOURCE_DIR}/glue/android_native_app_glue.c"
)

target_link_libraries(white_screen GLESv3 EGL android)
