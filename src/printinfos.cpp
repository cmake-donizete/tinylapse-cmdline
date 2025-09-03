#include <iostream>
#include <stdexcept>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

int main(int argc, char *argv[])
{
    std::string input = "../../frames/input.mp4";

    AVFormatContext *context = avformat_alloc_context();
    if (avformat_open_input(
            &context,
            input.c_str(),
            nullptr,
            nullptr) < 0)
    {
        throw std::invalid_argument("Something went wrong");
    }
    std::cout << "Input container name: " << context->iformat->name << '\n';
    std::cout << "Input size: " << context->duration << '\n';

    if (avformat_find_stream_info(context, nullptr) < 0)
    {
        throw std::invalid_argument("Something went wrong (again)");
    }

    for (int i = 0; i < context->nb_streams; i++)
    {
        auto stream = context->streams[i];
        auto codecparam = stream->codecpar;
        auto codec = avcodec_find_decoder(codecparam->codec_id);
        std::cout << "Stream " << i << " name: " << codec->name << '\n';
    }

    avformat_free_context(context);
    return EXIT_SUCCESS;
}