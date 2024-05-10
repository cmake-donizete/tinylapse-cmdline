add_executable(
    encode_video
    main.c
)

target_link_libraries(
    encode_video
    avcodec
    avformat
)