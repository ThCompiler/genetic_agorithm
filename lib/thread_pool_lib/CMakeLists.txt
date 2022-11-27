cmake_minimum_required(VERSION 3.1x)

set(PROJECT_NAME thread_pool_lib)
set(LIBRARY_NAME thread_pool_lib)

connect_lib(${LIBRARY_NAME} ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} pthread)