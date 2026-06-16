cmake_minimum_required(VERSION 3.28.3)

project(RELAY)

add_executable(${PROJECT_NAME}_server server.cpp)
add_executable(${PROJECT_NAME}_client client.cpp)
