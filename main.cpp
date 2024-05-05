extern "C"
{
#include <libswresample/swresample.h>
#include <getopt.h>
}

#include <string>
#include <iostream>
#include <vector>

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

int main(int argc, char *argv[])
{
    struct args args = {
        .framerate = 1,
        .directory = "",
    };

    parse_args(argc, argv, args);

    return EXIT_SUCCESS;
}