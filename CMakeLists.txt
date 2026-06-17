cmake_minimum_required(VERSION 3.28.3)

project(Relay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(relay_common
    src/common.cpp
)

target_include_directories(relay_common
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

add_executable(relay_server
    src/server_main.cpp
    src/server.cpp
)

target_link_libraries(relay_server
    PRIVATE
        relay_common
)

add_executable(relay_client
    src/client_main.cpp
    src/client.cpp
)

target_link_libraries(relay_client
    PRIVATE
        relay_common
)
