cmake_minimum_required(VERSION 3.16)
project(piper_my LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ALSA REQUIRED)

add_executable(app main.cpp my_piper.cpp alsa.cpp)

target_include_directories(app PRIVATE
    ${CMAKE_SOURCE_DIR}/../piper1-gpl/libpiper/include
)

target_link_libraries(app PRIVATE
    ${CMAKE_SOURCE_DIR}/../piper1-gpl/libpiper/build/libpiper.so
    ${CMAKE_SOURCE_DIR}/../piper1-gpl/libpiper/lib/onnxruntime-linux-x64-1.22.0/lib/libonnxruntime.so
    ALSA::ALSA
)

set_target_properties(app PROPERTIES
    BUILD_RPATH "${CMAKE_SOURCE_DIR}/../piper1-gpl/libpiper/build;${CMAKE_SOURCE_DIR}/../piper1-gpl/libpiper/lib/onnxruntime-linux-x64-1.22.0/lib"
)


