cmake_minimum_required(VERSION 3.16)
project(Assi CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Найти библиотеки
find_package(ALSA REQUIRED)


# Подключить whisper.cpp как подпроект
add_subdirectory(../whisper.cpp whisper_lib)
add_subdirectory(../llama.cpp llama_build)

# Твой исполняемый файл
add_executable(app main.cpp 
    ../wisper_my/my_whisper.cpp 
    ../llama_my/my_llama.cpp
    ../piper_my/my_piper.cpp
    ../piper_my/alsa.cpp )

#  whisper, alsa , piper ,llama
target_link_libraries(app PRIVATE whisper ${ALSA_LIBRARIES})
target_link_libraries(app PRIVATE llama)
target_link_libraries(app PRIVATE
    ${CMAKE_SOURCE_DIR}/../piper1-gpl/libpiper/build/libpiper.so
    ${CMAKE_SOURCE_DIR}/../piper1-gpl/libpiper/lib/onnxruntime-linux-x64-1.22.0/lib/libonnxruntime.so
    ALSA::ALSA
)
target_include_directories(app PRIVATE ../whisper.cpp)
target_include_directories(app PRIVATE ../wisper_my)
target_include_directories(app PRIVATE ../llama.cpp)
target_include_directories(app PRIVATE ../llama_my)
target_include_directories(app PRIVATE ../piper_my)

target_include_directories(app PRIVATE ../llama.cpp/ggml/include)
target_include_directories(app PRIVATE ../piper1-gpl/libpiper/include)


set_target_properties(app PROPERTIES
    BUILD_RPATH "${CMAKE_SOURCE_DIR}/../piper1-gpl/libpiper/build;${CMAKE_SOURCE_DIR}/../piper1-gpl/libpiper/lib/onnxruntime-linux-x64-1.22.0/lib"
)