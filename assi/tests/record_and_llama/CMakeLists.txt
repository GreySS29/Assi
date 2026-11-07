cmake_minimum_required(VERSION 3.16)
project(Assi CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ALSA REQUIRED)



add_subdirectory(../../../whisper.cpp whisper_lib)
add_subdirectory(../../../llama.cpp llama_build)


add_executable(main main.cpp 
    ../../src/record.cpp
    ../../src/llama.cpp )


target_link_libraries(main PRIVATE whisper ${ALSA_LIBRARIES})
target_include_directories(main PRIVATE ../../../whisper.cpp)
target_include_directories(main PRIVATE ../../../llama.cpp)
target_include_directories(main PRIVATE ../../../llama.cpp/ggml/include)
target_link_libraries(main PRIVATE llama)