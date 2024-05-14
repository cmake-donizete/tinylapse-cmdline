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
    std::string output;
};

static struct option long_options[] = {
    {"framerate", required_argument, 0, 'f'},
    {"directory", required_argument, 0, 'd'},
    {"output", required_argument, 0, 'o'},
};

static inline struct args parse_args(int argc, char **argv)
{
    struct args args;
    char c;
    int option_index;

    while ((c = getopt_long(argc, argv, "f:d:o:", long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'f':
            args.framerate = std::stoi(optarg);
            break;

        case 'd':
            args.directory = optarg;
            break;

        case 'o':
            args.output = optarg;
            break;
        }
    }

    if (args.directory.empty())
    {
        throw std::invalid_argument("Directory should not be empty");
    }

    return args;
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

    std::ranges::sort(frames, std::less());

    return frames;
}

int main(int argc, char *argv[])
{
    av_log_set_level(AV_LOG_VERBOSE);
    int ret = 0;
    struct args args = parse_args(argc, argv);

    const AVOutputFormat *oformat = av_guess_format(
        nullptr,
        args.output.c_str(),
        nullptr);

    AVFormatContext *fcontext = avformat_alloc_context();
    fcontext->oformat = oformat;

    const AVCodec *video_codec = avcodec_find_encoder(oformat->video_codec);

    AVCodecContext *video_codec_context = avcodec_alloc_context3(video_codec);
    video_codec_context->bit_rate = 40000;
    video_codec_context->width = 1280;
    video_codec_context->height = 720;
    video_codec_context->time_base = AVRational{.num = 1, .den = 1};
    video_codec_context->gop_size = 12; // emit one intra frame every twelve frames at most
    video_codec_context->pix_fmt = AV_PIX_FMT_YUV420P;

    AVStream *video_stream = avformat_new_stream(fcontext, nullptr);
    ret = avcodec_parameters_from_context(video_stream->codecpar, video_codec_context);

    ret = avcodec_open2(video_codec_context, video_codec, nullptr);
    ret = avio_open(&fcontext->pb, args.output.c_str(), AVIO_FLAG_WRITE);
    ret = avformat_write_header(fcontext, nullptr);

    return EXIT_SUCCESS;
}