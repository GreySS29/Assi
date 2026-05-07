cmake_minimum_required(VERSION 3.16)
project(test_Whis+Lam CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Найти библиотеки
find_package(ALSA REQUIRED)


# Подключить whisper.cpp как подпроект
add_subdirectory(../whisper.cpp whisper_lib)
add_subdirectory(../llama.cpp llama_build)

# Твой исполняемый файл
add_executable(main main.cpp ../wisper_my/my_whisper.cpp ../llama_my/my_llama.cpp )

# Линкуем с whisper и alsa
target_link_libraries(main PRIVATE whisper ${ALSA_LIBRARIES})
target_include_directories(main PRIVATE ../whisper.cpp)
target_include_directories(main PRIVATE ../wisper_my)
target_include_directories(main PRIVATE ../llama.cpp)
target_include_directories(main PRIVATE ../llama_my)
target_include_directories(main PRIVATE ../llama.cpp/ggml/include)
target_link_libraries(main PRIVATE llama)