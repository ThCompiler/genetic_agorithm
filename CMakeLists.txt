cmake_minimum_required(VERSION 3.1X)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wpedantic -Werror -Wextra -ggdb3")

set(PROJECT_NAME nn_project)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(SOURCE ${SOURCE_DIR}/main.cpp lib/genetic_lib/genetic.hpp)
set(LIBRARIES_DIR ${CMAKE_SOURCE_DIR}/lib)
cmake_policy(SET CMP0079 NEW)

project(${PROJECT_NAME})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-utilits")
include(templates)

###########
# Project #
###########

#find_package(SFML 2.5 COMPONENTS network audio graphics window system REQUIRED)
#include_directories(${SFML_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${SOURCE})

set(SFML_STATIC_LIBRARIES true)

add_subdirectory("lib/genetic_lib")
add_subdirectory("lib/thread_pool_lib")

target_link_libraries(evolution_lib thread_pool_lib)

target_link_libraries(${PROJECT_NAME} evolution_lib)

#target_link_libraries(${PROJECT_NAME} -lsfml-graphics -lsfml-window -lsfml-system)