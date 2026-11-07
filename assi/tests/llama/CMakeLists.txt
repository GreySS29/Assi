cmake_minimum_required(VERSION 3.16)
project(Assi CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



add_subdirectory(../../../llama.cpp llama_build)

add_executable(main 
    main.cpp 
    ../../src/llama.cpp)

target_include_directories(main PRIVATE ../../../llama.cpp)
target_include_directories(main PRIVATE ../../../llama.cpp/ggml/include)
target_link_libraries(main PRIVATE llama)