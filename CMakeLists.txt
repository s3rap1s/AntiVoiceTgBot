cmake_minimum_required(VERSION 3.20)
project(AntiVoiceBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/bot/commands.cpp
    src/bot/event_handlers.cpp
    src/core/gradual_editor.cpp
    src/core/message_storage.cpp
    src/core/task_manager.cpp
    src/core/user_storage.cpp
    src/utils/speed.cpp
    src/utils/text_utils.cpp
    src/main.cpp
)

find_package(TgBot REQUIRED) 

add_executable(${PROJECT_NAME} ${SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/src)


target_link_libraries(${PROJECT_NAME} PRIVATE TgBot::TgBot)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic -Werror=format-security -Wno-unused-parameter -Wc++20-compat -Wregister -Wclass-memaccess -Wduplicated-cond
)
