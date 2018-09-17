#include <iostream>
#include <bitstream/ifstream.h>
#include <bitstream/opstream.h>
#include <mpeg-isobase/parser.h>
#include <mpeg-profiler/profiler.h>


int main(int argc, const char **argv) {
    for (int i = 1; i < argc; ++i) {
        try {
            bitstream::input::file::Stream stream(argv[i]);

            mpeg::profiler::Profiler profiler;
            mpeg::isobase::Parser parser(stream, profiler);

            parser.parse();

            auto container_ptr = profiler.detach_container();
            const auto &container = *container_ptr;
            bitstream::output::print::to_stdout << container;

        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

