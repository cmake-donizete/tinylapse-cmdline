cmake_minimum_required(VERSION 3.29.2)

project(tinylapse-cmdline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(INTERNAL_DEBUG FALSE)

if(INTERNAL_DEBUG)
    execute_process(COMMAND sh ../scripts/build_ffmpeg_debug.sh)

    link_directories(/tmp/local/lib)
    include_directories(/tmp/local/include)
else()
    if (APPLE)
        link_directories(/opt/homebrew/lib)
        include_directories(/opt/homebrew/include)
    endif()
endif()

add_executable(
    tinylapse-cmdline
    main.cpp
)

target_link_libraries(
    tinylapse-cmdline
    avcodec
    avformat
    avutil
)