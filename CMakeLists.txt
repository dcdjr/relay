cmake_minimum_required(VERSION 3.28.3)

project(Relay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(relay_server
    src/server_main.cpp
    src/server.cpp
)

add_executable(relay_client
    src/client_main.cpp
    src/client.cpp
)

target_include_directories(relay_server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_include_directories(relay_client
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
