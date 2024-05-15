extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
}

#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <getopt.h>

#define WIDTH 1280
#define HEIGHT 720

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
    video_codec_context->bit_rate = 400000;
    video_codec_context->width = WIDTH;
    video_codec_context->height = HEIGHT;
    video_codec_context->time_base = AVRational{.num = 1, .den = 1};
    video_codec_context->gop_size = 12; // emit one intra frame every twelve frames at most
    video_codec_context->pix_fmt = AV_PIX_FMT_YUV420P;

    AVStream *video_stream = avformat_new_stream(fcontext, nullptr);
    ret = avcodec_parameters_from_context(video_stream->codecpar, video_codec_context);

    ret = avcodec_open2(video_codec_context, video_codec, nullptr);
    ret = avio_open(&fcontext->pb, args.output.c_str(), AVIO_FLAG_WRITE);
    ret = avformat_write_header(fcontext, nullptr);

    AVFrame *target_frame = av_frame_alloc();
    target_frame->width = video_codec_context->width;
    target_frame->height = video_codec_context->height;
    target_frame->format = video_codec_context->pix_fmt;

    ret = av_image_alloc(
        target_frame->data,
        target_frame->linesize,
        target_frame->width,
        target_frame->height,
        (AVPixelFormat)(target_frame->format),
        32);

    for (std::string &frame_name : find_frames(args.directory))
    {
        AVFormatContext *frame_format_context = nullptr;
        ret = avformat_open_input(&frame_format_context, frame_name.c_str(), nullptr, nullptr);
        ret = avformat_find_stream_info(frame_format_context, nullptr);

        const AVCodec *frame_codec = nullptr;
        ret = av_find_best_stream(frame_format_context, AVMediaType::AVMEDIA_TYPE_VIDEO, -1, -1, &frame_codec, 0);

        AVCodecContext *frame_codec_context = nullptr;
        frame_codec_context = avcodec_alloc_context3(frame_codec);
        avcodec_parameters_to_context(frame_codec_context, frame_format_context->streams[0]->codecpar);

        ret = avcodec_open2(frame_codec_context, frame_codec, nullptr);

        AVFrame *frame = nullptr;
        frame = av_frame_alloc();

        AVPacket *packet = nullptr;
        packet = av_packet_alloc();

        while (av_read_frame(frame_format_context, packet) >= 0)
        {
            if (packet->stream_index == 0)
            {
                ret = avcodec_send_packet(frame_codec_context, packet);
                if (ret < 0)
                    continue;
                ret = avcodec_receive_frame(frame_codec_context, frame);
                if (ret == 0)
                    break;
            }
            av_packet_unref(packet);
        }

        av_packet_free(&packet);
        avformat_free_context(frame_format_context);
        avcodec_free_context(&frame_codec_context);
    }

    av_write_trailer(fcontext);

    return EXIT_SUCCESS;
}