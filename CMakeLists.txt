cmake_minimum_required(VERSION 3.29.2)

project(tinylapse-cmdline)

add_executable(
    tinylapse-cmdline
    main.cpp
)

target_link_libraries(
    tinylapse-cmdline
    /opt/homebrew/lib/libswresample.dylib
)

target_include_directories(
    tinylapse-cmdline
    PUBLIC
    /opt/homebrew/include/
)