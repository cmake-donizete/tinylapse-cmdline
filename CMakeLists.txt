cmake_minimum_required(VERSION 3.29.2)

project(tinylapse-cmdline)

set(CMAKE_CXX_STANDARD 20)
set(INTERNAL_DEBUG TRUE)

if (INTERNAL_DEBUG)
    execute_process(
        COMMAND sh ../scripts/build_ffmpeg_debug.sh
    )
elseif (APPLE)
    link_directories(/opt/homebrew/lib)
    include_directories(/opt/homebrew/include)
endif()

add_subdirectory(examples)

add_executable(
    tinylapse-cmdline
    main.cpp
)

target_link_libraries(
    tinylapse-cmdline
    avcodec
    avformat
)