extern "C" {
    #include <libswresample/swresample.h>
}

int main(int argc, char* argv[]) {
    struct SwrContext* context = swr_alloc();
    return EXIT_SUCCESS;
}