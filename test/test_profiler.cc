#include <gtest/gtest.h>
#include <bitstream/ifstream.h>
#include <bitstream/opstream.h>
#include <mpeg-mp4/parser.h>
#include <mpeg-mp4/boxes.h>
#include <mpeg-profiler/profiler.h>
#include <mpeg-profiler/box/track.h>
#include <mpeg-profiler/box/movie_header.h>
#include <mpeg-profiler/box/media_header.h>
#include <mpeg-profiler/box/media_samples_iterator.h>
#include "file_list_processing.h"


TEST(Profiler, dev) {
    struct: mpeg::isobase::test::FileListProcessing {
        virtual void process(const std::string &file) {
            bitstream::input::file::Stream stream(file);
            mpeg::profiler::Profiler profiler;
            mpeg::mp4::Parser parser(stream, profiler);
            parser.parse();

            auto container_ptr = profiler.detach_container();
            const auto &container = *container_ptr;

            std::cout << std::endl << file << ":" << std::endl;
            bitstream::output::print::to_stdout << container;

            {
                using namespace mpeg::profiler;
                {
                    const auto &mvhd = container['moov']['mvhd'];
                }{
                    const auto &mvhd = container['moov'].as<Movie::Header>('mvhd');
                }{
                    assert(container.get('foo ') == nullptr);
                }{
                    struct Foo: Box {};
                    const Foo *foo = container.get<Foo>('bar');
                }{
                    for (const auto &track: container['moov'].all('trak')) {
                        track['mdia'].as<Media::Header>('mdhd');
                    }
                }
            }

            for (const auto &track: container.tracks()) {

                const auto &samples = track.samples();
                assert((samples.end() - samples.begin()) == samples.count());

                auto i = 0;
                for (auto it = samples.begin(); it != samples.end(); ++it, ++i) {
                    const auto atit = samples.at(i);
                    assert(atit == it);
                    assert(it == atit);
                    assert(*it == *atit);
                    assert(*atit == *it);
                }

                if (samples.sync() != nullptr) {
                    for (auto sample_index: samples.sync()->entries) {
                        const auto it = samples.at(sample_index);
                        auto decoding_time = (*it).decoding_time();
                        assert(samples.seek(decoding_time) == sample_index);
                    }
                }
            }
        }
    } processing;
    processing.process_files();
}

