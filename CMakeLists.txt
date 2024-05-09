cmake_minimum_required(VERSION 3.29.2)

project(tinylapse-cmdline)

set (CMAKE_CXX_STANDARD 20)

if (APPLE)
    link_directories(/opt/homebrew/lib)
    include_directories(/opt/homebrew/include)
endif()

add_subdirectory(hello_world)

add_executable(
    tinylapse-cmdline
    main.cpp
)

target_link_libraries(
    tinylapse-cmdline
    swresample
)