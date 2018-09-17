#include <bitstream/opstream.h>
#include <mpeg-isobase/box/sample_to_chunk.h>
#include <mpeg-profiler/profiler.h>
#include <mpeg-profiler/box/media_samples_size.h>
#include <mpeg-profiler/box/media_samples_chunk_offset.h>
#include <mpeg-profiler/box/media_samples_description.h>
#include <mpeg-profiler/box/media_samples_tochunk.h>


namespace mpeg {
namespace profiler {


using bitstream::SStream;
using bitstream::output::print::fourcc;
using bitstream::output::print::pluralize;


void Media::Samples::ToChunk::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    const auto &box = cast_header<isobase::SampleToChunkBox>(event);
    box.parsing_control().skip_payload_parsing = true;   // do that explicitly

    _runs_count = box.entry_count;
    runs.reserve(_runs_count);
}

void Media::Samples::ToChunk::event(const bitstream::Parser::Event::Payload::Boundary::Begin &event) {
    for(auto box_run: isobase::SampleToChunkBox::Parsed::Entries{event, _runs_count}) {
        const uint32_t first_chunk = box_run.first_chunk;
        if (0 == first_chunk) {
            throw Profiler::Exception(SStream()
                    << "SampleToChunk run[" << runs.size() << "].first_chunk=0" << " but must be greater.");
        }
        const uint32_t first_chunk_index = first_chunk - 1;

        uint32_t sample_description_index = box_run.sample_description_index;
        if (0 == sample_description_index) {
            throw Profiler::Exception(SStream()
                    << "SampleToChunk run[" << runs.size() << "].sample_description_index=0" << " but must be greater.");
        }
        --sample_description_index;

        update_last_run_and_totals(first_chunk_index);
        runs.emplace_back(Run{
                first_chunk_index,
                box_run.samples_per_chunk,
                sample_description_index});
    }
}

inline void Media::Samples::ToChunk::update_last_run_and_totals(uint32_t next_first_chunk_index) {
    if (!runs.empty()) {
        auto &last_run = runs.back();
        if (total.chunks >= next_first_chunk_index) {
            throw Profiler::Exception(SStream()
                    << "SampleToChunk runs are not strictly inreased run[" << runs.size() << "].first_chunk_index="
                    << total.chunks << " suppose to be less then following first_chunk=" << next_first_chunk_index);
        }
        last_run.chunks = next_first_chunk_index - total.chunks;
        last_run.sample_index = total.samples;
        total.chunks = next_first_chunk_index;
        total.samples += last_run.chunks * last_run.samples_per_chunk;
    }
}

void Media::Samples::ToChunk::operator >> (isobase::SampleToChunkBox &box) const {
    FullBox::operator >> (box);

    isobase::SampleToChunkBox::Composed::Entries runs{box, this->runs.size()};
    auto i = 0;
    for(auto box_run : runs) {
        const auto &run = this->runs[i++];
        box_run.first_chunk = run.first_chunk_index + 1;
        box_run.samples_per_chunk = run.samples_per_chunk;
        box_run.sample_description_index = run.sample_description_index + 1;
    }
}

void Media::Samples::ToChunk::operator >> (OPrintStream &stream) const {
    stream.out << "Samples to chunks: ["
        << total.samples << " " << pluralize("sample", total.samples) << " (in "
        << runs.size() << " " << pluralize("run", runs.size()) <<")]";
}

void Media::Samples::ToChunk::seal() {
    const auto &size = parent.any<Samples::Size>('stsz', 'stz2');
    const auto &stco = parent.any<Media::Samples::ChunkOffset>('stco', 'co64');
    update_last_run_and_totals(stco.chunk_count());
    if (sample_count() != size.sample_count()) {
        throw Profiler::Exception(SStream()
                << "Samples count mismatch ChunkToSample('stsc')["
                << sample_count() << " " << pluralize("sample", sample_count())
                << "] != Size(" << fourcc(size.type) << ")["
                << size.sample_count() << " " << pluralize("sample", size.sample_count()) << "]");
    }
    const auto &description = parent.as<Description>('stsd');
    for (const auto &run: runs) {
        if (run.sample_description_index >= description.all().size()) {
            throw Profiler::Exception(SStream()
                    << "SampleToChunk: description reference index " << run.sample_description_index
                    << " exceeds total number of descriptions " << description.all().size() << " " << pluralize("item", description.all().size()));
        }
    }
}


}} // namespace mpeg::profiler

