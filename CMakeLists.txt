cmake_minimum_required(VERSION 4.1.1)

project(tinylapse-cmdline LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(INTERNAL_DEBUG OFF)

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

macro(ffmpeg_executable)
    set(PATH ${ARGV0})
    cmake_path(GET PATH STEM NAME)
    add_executable(${NAME} ${PATH})
    target_link_libraries(
        ${NAME}
        avcodec
        avformat
        avutil
    )
endmacro(ffmpeg_executable)

ffmpeg_executable(main.cpp)
ffmpeg_executable(src/printinfos.cpp)

ffmpeg_executable(external/FFmpeg/doc/examples/encode_video.c)