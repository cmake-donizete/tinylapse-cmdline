extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <ranges>
#include <getopt.h>

struct args
{
    char framerate;
    std::string directory;
};

static struct option long_options[] = {
    {"framerate", required_argument, 0, 'f'},
    {"directory", required_argument, 0, 'd'},
};

static void parse_args(int argc, char **argv, struct args &args)
{
    char c;
    int option_index;

    while ((c = getopt_long(argc, argv, "f:d:", long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'f':
            args.framerate = std::stoi(optarg);
            break;

        case 'd':
            args.directory = optarg;
            break;
        }
    }
}

static inline std::vector<std::string> find_frames(std::string &folder)
{
    std::vector<std::string> frames;

    for (auto &data : std::filesystem::directory_iterator(folder))
    {
        if (!data.is_regular_file())
            continue;
        frames.push_back(data.path());
    }

    std::ranges::sort(
        frames,
        std::less());

    return frames;
}

int main(int argc, char *argv[])
{
    int ret;
    struct args args = {
        .framerate = 1,
        .directory = "",
    };

    parse_args(argc, argv, args);

    if (args.directory.empty())
    {
        throw std::invalid_argument("Directory should not be empty");
    }

    auto frames = find_frames(args.directory);

    std::string outputName = "output.mp4";
    FILE *outputFile = fopen(outputName.c_str(), "wb");

    auto avCodec = avcodec_find_encoder(AVCodecID::AV_CODEC_ID_MPEG4);
    auto avCodecContext = avcodec_alloc_context3(avCodec);
    avCodecContext->gop_size = 10;
    avCodecContext->max_b_frames = 1;
    avCodecContext->bit_rate = 400000;
    avCodecContext->width = 1280;
    avCodecContext->height = 720;
    avCodecContext->time_base = AVRational{.num = 1, .den = 25};
    avCodecContext->framerate = AVRational{.num = 25, .den = 1};
    avCodecContext->pix_fmt = AVPixelFormat::AV_PIX_FMT_YUV420P;
    ret = avcodec_open2(avCodecContext, avCodec, nullptr);

    auto avFrame = av_frame_alloc();
    avFrame->format = avCodecContext->pix_fmt;
    avFrame->width = avCodecContext->width;
    avFrame->height = avCodecContext->height;
    ret = av_frame_get_buffer(avFrame, 0);

    auto avPacket = av_packet_alloc();

    for (int i = 0; i < 25; i++)
    {
        ret = av_frame_make_writable(avFrame);

        /* Prepare a dummy image.
           In real code, this is where you would have your own logic for
           filling the frame. FFmpeg does not care what you put in the
           frame.
         */
        /* Y */
        for (int y = 0; y < avCodecContext->height; y++)
        {
            for (int x = 0; x < avCodecContext->width; x++)
            {
                avFrame->data[0][y * avFrame->linesize[0] + x] = x + y + i * 3;
            }
        }

        /* Cb and Cr */
        for (int y = 0; y < avCodecContext->height / 2; y++)
        {
            for (int x = 0; x < avCodecContext->width / 2; x++)
            {
                avFrame->data[1][y * avFrame->linesize[1] + x] = 128 + y + i * 2;
                avFrame->data[2][y * avFrame->linesize[2] + x] = 64 + x + i * 5;
            }
        }

        avFrame->pts = i;

        ret = avcodec_send_frame(avCodecContext, avFrame);
        ret = avcodec_receive_packet(avCodecContext, avPacket);
        if (ret < 0)
        {
            throw std::runtime_error("Something went wrong");
        }
        if (ret == AVERROR(EAGAIN) || AVERROR_EOF)
            continue;
        fwrite(avPacket->data, 1, avPacket->size, outputFile);
    }

    avcodec_free_context(&avCodecContext);
    av_packet_free(&avPacket);
    av_frame_free(&avFrame);

    return EXIT_SUCCESS;
}