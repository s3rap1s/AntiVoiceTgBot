cmake_minimum_required(VERSION 3.20)
project(AntiVoiceBot VERSION 1.0.0 LANGUAGES CXX)

cmake_policy(SET CMP0167 NEW)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/include)
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE SRC_FILES 
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*.cc"
)

if(NOT SRC_FILES)
    message(FATAL_ERROR "No source files found in ${SRC_DIR}! Check directory structure.")
endif()
message(STATUS "Found source files: ${SRC_FILES}")

# Зависимости
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS headers)
find_package(TgBot REQUIRED)
find_package(PkgConfig REQUIRED)

add_executable(${PROJECT_NAME} ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
    OUTPUT_NAME "bot"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE 
        ${INC_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        TgBot::TgBot
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic -Werror=format-security -Wno-unused-parameter
)

message(STATUS "Include directories: ${INC_DIR}")
get_target_property(INCLUDES ${PROJECT_NAME} INCLUDE_DIRECTORIES)
message(STATUS "Target includes: ${INCLUDES}")
