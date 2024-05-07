extern "C"
{
#include <libswresample/swresample.h>
#include <getopt.h>
}

#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <ranges>

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

    return EXIT_SUCCESS;
}