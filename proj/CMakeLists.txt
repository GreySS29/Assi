cmake_minimum_required(VERSION 3.20)
project(VoicePipeline CXX)

set(CMAKE_CXX_STANDARD 17)

# Создаём исполняемый файл
add_executable(voice_pipeline main.cpp)


# Находим pkg-config для ALSA и CURL
find_package(PkgConfig REQUIRED)
pkg_check_modules(ALSA REQUIRED alsa)
pkg_check_modules(CURL REQUIRED libcurl)


# Правильные пути (относительно build/)
set(LLAMA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../llama.cpp")
set(WHISPER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../whisper.cpp")


# Проверка существования репозиториев
if(NOT EXISTS "${LLAMA_PATH}")
    message(FATAL_ERROR "llama.cpp не найден: ${LLAMA_PATH}")
endif()
if(NOT EXISTS "${WHISPER_PATH}")
    message(FATAL_ERROR "whisper.cpp не найден: ${WHISPER_PATH}")
endif()


# Include пути — КЛЮЧЕВОЕ исправление!
target_include_directories(voice_pipeline PRIVATE
    "${WHISPER_PATH}/include"
    "${LLAMA_PATH}/include"
    "${WHISPER_PATH}/ggml/include"

    ${ALSA_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
)



# Линкуем библиотеки
target_link_libraries(voice_pipeline PRIVATE
    "${WHISPER_PATH}/build/libwhisper.a"
    "${LLAMA_PATH}/build/bin/libllama.so"
    ${ALSA_LIBRARIES}
    ${CURL_LIBRARIES}
    pthread m dl
)

# Компиляторные флаги
target_compile_options(voice_pipeline PRIVATE
    ${ALSA_CFLAGS_OTHER}
    ${CURL_CFLAGS_OTHER}
    -O3
)
