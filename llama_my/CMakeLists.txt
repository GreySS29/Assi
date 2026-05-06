cmake_minimum_required(VERSION 3.16)
project(Llama CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



# Подключить whisper.cpp как подпроект
add_subdirectory(../llama.cpp llama_build)

# Твой исполняемый файл
add_executable(main main.cpp)

target_include_directories(main PRIVATE ../llama.cpp)
target_include_directories(main PRIVATE ../llama.cpp/ggml/include)
target_link_libraries(main PRIVATE llama)