cmake_minimum_required(VERSION 3.20)
project(AntiVoiceBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES src/*.cpp)

find_package(TgBot REQUIRED) 

add_executable(AntiVoiceBot ${SOURCES})

target_include_directories(AntiVoiceBot PRIVATE include)

target_link_libraries(AntiVoiceBot PRIVATE TgBot::TgBot)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic -Werror=format-security -Wno-unused-parameter
)