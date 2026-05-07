cmake_minimum_required(VERSION 3.16)
project(VoiceWhisper CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Найти библиотеки
find_package(ALSA REQUIRED)

# Подключить whisper.cpp как подпроект
add_subdirectory(../whisper.cpp whisper_lib)

# Твой исполняемый файл
add_executable(main main.cpp my_whisper.cpp)

# Линкуем с whisper и alsa
target_link_libraries(main PRIVATE whisper ${ALSA_LIBRARIES})
target_include_directories(main PRIVATE ../whisper.cpp)
