#include <iostream>
#include <iomanip>
#include <bitstream/ifstream.h>
#include <bitstream/opstream.h>
#include <mpeg-isobase/parser.h>
#include <mpeg-profiler/profiler.h>
#include <mpeg-profiler/box/track.h>
#include <mpeg-profiler/box/media_samples_iterator.h>


int main(int argc, const char **argv) {
    auto &std_out = bitstream::output::print::to_stdout;
    std::cout << std::setprecision(2) << std::fixed;

    for (int i = 1; i < argc; ++i) {
        try {
            bitstream::input::file::Stream stream(argv[i]);

            mpeg::profiler::Profiler profiler;
            mpeg::isobase::Parser parser(stream, profiler);

            parser.parse();

            auto container_ptr = profiler.detach_container();
            const auto &container = *container_ptr;
            for (auto no_sync = 0; no_sync < 2; ++no_sync) {
                std::cout << std::endl << "no_sync: " << bool(no_sync) << std::endl;
                for (const auto &track: container.tracks()) {
                    const auto &samples = track.samples();
                    const auto begin = samples.seek(5.7, bool(no_sync)), end = samples.seek(6.3, bool(no_sync));

                    std_out << track['mdia']['hdlr'];
                    std::cout << ": " << end - begin << " samples:" << std::endl;
                    auto i = 0;
                    for (auto it = samples.at(begin); it != end; ++it) {
                        const auto &sample = *it;
                        std::cout
                            << "#" << std::setw(3) << std::left <<  i++ << ": ["
                            << "time: " << sample.decoding_time().seconds() << "s, "
                            << "size: " << sample.size() << " bytes, "
                            << "offset: " << sample.offset() << " bytes]" << std::endl;
                    }
                }
            }

        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

